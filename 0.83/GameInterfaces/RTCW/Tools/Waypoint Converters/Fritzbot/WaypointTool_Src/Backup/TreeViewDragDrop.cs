using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using System.Data;
using System.Runtime.InteropServices;
using System.Windows.Forms.Design;

namespace WaypointTool
{
	// - Implements:
	//   + Auto scrolling
	//   + Target node highlighting when over a node
	//   + Custom cursor when dragging
	//   + Custom ghost icon + label when dragging
	//   + Escape key to cancel drag
	//	 + Blocks certain nodes from being dragged via cancel event
	//   + Sanity checks for dragging (no parent into children nodes, target isn't the source)

	// Gotchas:
	// - Explorer can tell if you have the treeview node selected or not
	// - The drag icon has to be dragged to the right, not in the center (or the form has 
	//  a fight with the treeview over focus)
	// - No auto opening of items

	#region TreeViewDragDrop class
	/// <summary>
	/// A treeview with inbuilt drag-drop support and custom cursor/icon dragging.
	/// </summary>
	[
	ToolboxBitmap(typeof(TreeViewDragDrop), "Sloppycode.UI.DragDropTreeView.bmp"),
	Description("A treeview with inbuilt drag-drop support and custom cursor/icon dragging.")	
	]
	public class TreeViewDragDrop : System.Windows.Forms.TreeView
	{
		#region Win32 api import, events
		[DllImport("user32.dll")]
		private static extern int SendMessage (IntPtr hWnd, int wMsg, IntPtr wParam,int lParam);

		/// <summary>
		/// Occurs when an item is starting to be dragged. This
		/// event can be used to cancel dragging of particular items.
		/// </summary>
		[
		Description("Occurs when an item is starting to be dragged. This event can be used to cancel dragging of particular items."),
		]
		public event DragItemEventHandler DragStart;

		/// <summary>
		/// Occurs when an item is dragged and dropped onto another.
		/// </summary>
		[
		Description("Occurs when an item is dragged and dropped onto another."),
		]
		public event DragCompleteEventHandler DragComplete;
		
		/// <summary>
		/// Occurs when an item is dragged, and the drag is cancelled.
		/// </summary>
		[
		Description("Occurs when an item is dragged, and the drag is cancelled."),
		]
		public event DragItemEventHandler DragCancel;

		
		#endregion

		#region Public properties
		/// <summary>
		/// The imagelist control from which DragImage icons are taken.
		/// </summary>
		[
		Description("The imagelist control from which DragImage icons are taken."),
		Category("Drag and drop")
		]
		public ImageList DragImageList
		{
			get
			{
				return this._formDrag.imageList1;
			}
			set
			{
				if ( value == this._formDrag.imageList1 )
				{
					return;
				}

				this._formDrag.imageList1 = value;

				// Change the picture box to use this image
				if ( this._formDrag.imageList1.Images.Count > 0 && this._formDrag.imageList1.Images[this._dragImageIndex] != null )
				{
					this._formDrag.pictureBox1.Image = this._formDrag.imageList1.Images[this._dragImageIndex];
					this._formDrag.Height = this._formDrag.pictureBox1.Image.Height;
				}

				if ( !base.IsHandleCreated )
				{
					return;
				}
				SendMessage((IntPtr) 4361, 0, ((value == null) ? IntPtr.Zero : value.Handle),0);
			}

		}

		/// <summary>
		/// The default image index for the DragImage icon.
		/// </summary>
		[
		Description("The default image index for the DragImage icon."),
		Category("Drag and drop"),
		TypeConverter(typeof(ImageIndexConverter)), 
		Editor("System.Windows.Forms.Design.ImageIndexEditor",typeof(System.Drawing.Design.UITypeEditor))
		]
		public int DragImageIndex
		{
			get
			{
				if ( this._formDrag.imageList1 == null)
				{
					return -1;
				}

				if ( this._dragImageIndex >= this._formDrag.imageList1.Images.Count)
				{
					return Math.Max(0, (this._formDrag.imageList1.Images.Count - 1));
				}
				else

					return this._dragImageIndex;
			}
			set
			{
				// Change the picture box to use this image
				if ( this._formDrag.imageList1.Images.Count > 0 && this._formDrag.imageList1.Images[value] != null )
				{
					this._formDrag.pictureBox1.Image = this._formDrag.imageList1.Images[value];
					this._formDrag.Size = new Size(this._formDrag.Width,this._formDrag.pictureBox1.Image.Height);
					this._formDrag.labelText.Size = new Size(this._formDrag.labelText.Width,this._formDrag.pictureBox1.Image.Height);
				}

				this._dragImageIndex = value;
			}
		}
		
		/// <summary>
		/// The custom cursor to use when dragging an item, if DragCursor is set to Custom.
		/// </summary>
		[
		Description("The custom cursor to use when dragging an item, if DragCursor is set to Custom."),
		Category("Drag and drop")
		]
		public Cursor DragCursor
		{
			get
			{
				return this._dragCursor;
			}
			set
			{
				if ( value == this._dragCursor)
				{
					return;
				}

				this._dragCursor = value;
				if ( !base.IsHandleCreated )
				{
					return;
				}
			}
		}

		/// <summary>
		/// The cursor type to use when dragging - None uses the default drag and drop cursor, DragIcon uses an icon and label, Custom uses a custom cursor.
		/// </summary>
		[
		Description("The cursor type to use when dragging - None uses the default drag and drop cursor, DragIcon uses an icon and label, Custom uses a custom cursor."),
		Category("Drag and drop")
		]
		public DragCursorType DragCursorType
		{
			get
			{
				return this._dragCursorType;
			}
			set
			{
				this._dragCursorType = value;
			}
		}

		/// <summary>
		/// Sets the font for the dragged node (shown as ghosted text/icon).
		/// </summary>
		[
		Description("Sets the font for the dragged node (shown as ghosted text/icon)."),
		Category("Drag and drop")
		]
		public Font DragNodeFont
		{
			get
			{
				return this._formDrag.labelText.Font ;
			}
			set
			{
				this._formDrag.labelText.Font = value;

				// Set the drag form height to the font height
				this._formDrag.Size = new Size(this._formDrag.Width,(int) this._formDrag.labelText.Font.GetHeight());
				this._formDrag.labelText.Size = new Size(this._formDrag.labelText.Width,(int) this._formDrag.labelText.Font.GetHeight());
				

			}
		}

		/// <summary>
		/// Sets the opacity for the dragged node (shown as ghosted text/icon).
		/// </summary>
		[
		Description("Sets the opacity for the dragged node (shown as ghosted text/icon)."),
		Category("Drag and drop"),
		TypeConverter(typeof(System.Windows.Forms.OpacityConverter))
		]
		public double DragNodeOpacity
		{ 
			get
			{
				return this._formDrag.Opacity;
			}
			set
			{
				this._formDrag.Opacity = value;
			}
		}

		/// <summary>
		/// The background colour of the node being dragged over.
		/// </summary>
		[
		Description("The background colour of the node being dragged over."),
		Category("Drag and drop")
		]
		public Color DragOverNodeBackColor
		{
			get
			{
				return this._dragOverNodeBackColor;
			}
			set
			{
				this._dragOverNodeBackColor = value;
			}
		}

		/// <summary>
		/// The foreground colour of the node being dragged over.
		/// </summary>
		[
		Description("The foreground colour of the node being dragged over."),
		Category("Drag and drop")
		]
		public Color DragOverNodeForeColor
		{
			get
			{
				return this._dragOverNodeForeColor;
			}
			set
			{
				this._dragOverNodeForeColor = value;
			}
		}

		/// <summary>
		/// The drag mode (move,copy etc.)
		/// </summary>
		[
		Description("The drag mode (move,copy etc.)"),
		Category("Drag and drop")
		]
		public DragDropEffects DragMode
		{
			get
			{
				return this._dragMode;
			}
			set
			{
				this._dragMode = value;
			}
		}
		#endregion
		
		#region Private members
		private int _dragImageIndex;
		private DragDropEffects _dragMode = DragDropEffects.Move;
		private Color _dragOverNodeForeColor = SystemColors.HighlightText;
		private Color _dragOverNodeBackColor = SystemColors.Highlight;
		private DragCursorType _dragCursorType;
		private Cursor _dragCursor = null;
		private TreeNode _previousNode;
		private TreeNode _selectedNode;
		private FormDrag _formDrag = new FormDrag();
		#endregion

		#region Constructor
		public TreeViewDragDrop()
		{
			base.SetStyle(ControlStyles.DoubleBuffer,true);
			this.AllowDrop = true;

			// Set the drag form to have ambient properties
			this._formDrag.labelText.Font = this.Font;
			this._formDrag.BackColor = this.BackColor;

			// Custom cursor handling
			if ( this._dragCursorType == DragCursorType.Custom && this._dragCursor != null )
			{
				this.DragCursor = this._dragCursor; 
			}

			this._formDrag.Show();
			this._formDrag.Visible = false;
		}
		#endregion

		#region Over-ridden methods
		/// <summary>
		/// 
		/// </summary>
		/// <param name="m"></param>
		protected override void WndProc(ref Message m)
		{
			//System.Diagnostics.Debug.WriteLine(m);
			// Stop erase background message
			if (m.Msg == (int)0x0014 )
			{
				m.Msg = (int) 0x0000; // Set to null
			} 
			
			base.WndProc(ref m);
		}
	
		/// <summary>
		/// 
		/// </summary>
		/// <param name="e"></param>
		protected override void OnGiveFeedback(System.Windows.Forms.GiveFeedbackEventArgs e)
		{
			if ( e.Effect == this._dragMode )
			{
				e.UseDefaultCursors = false;

				if ( this._dragCursorType == DragCursorType.Custom && this._dragCursor != null )
				{
					// Custom cursor
					this.Cursor = this._dragCursor;
				}
				else if ( this._dragCursorType == DragCursorType.DragIcon )
				{
					// This removes the default drag + drop cursor
					this.Cursor = Cursors.Default;
				}
				else
				{
					e.UseDefaultCursors = true;
				}
			}
			else
			{
				e.UseDefaultCursors = true;
				this.Cursor = Cursors.Default;
			}
		}
	
		/// <summary>
		/// 
		/// </summary>
		/// <param name="e"></param>
		protected override void OnItemDrag(System.Windows.Forms.ItemDragEventArgs e)
		{
			this._selectedNode = (TreeNode) e.Item;

			// Call dragstart event
			if ( this.DragStart != null )
			{
				DragItemEventArgs ea = new DragItemEventArgs();
				ea.Node = this._selectedNode;

				this.DragStart(this,ea);
			}
			// Change any previous node back 
			if ( this._previousNode != null )
			{
				this._previousNode.BackColor = SystemColors.HighlightText;
				this._previousNode.ForeColor = SystemColors.ControlText;
			}

			// Move the form with the icon/label on it
			// A better width measurement algo for the form is needed here

			int width = Width  = this._selectedNode.Text.Length * (int) this._formDrag.labelText.Font.Size;
			if ( this._selectedNode.Text.Length < 5 )
				width += 20;

			this._formDrag.Size = new Size(width,this._formDrag.Height);

			this._formDrag.labelText.Size = new Size(width,this._formDrag.labelText.Size.Height);
			this._formDrag.labelText.Text = this._selectedNode.Text;

			// Start drag drop
			this.DoDragDrop(e.Item,this._dragMode);
		}
	
		/// <summary>
		/// 
		/// </summary>
		/// <param name="e"></param>
		protected override void OnDragOver(System.Windows.Forms.DragEventArgs e)
		{
			// Change any previous node back
			if ( this._previousNode != null )
			{
				this._previousNode.BackColor = SystemColors.HighlightText;
				this._previousNode.ForeColor = SystemColors.ControlText;
			}
			TreeNode treeNode = this.Nodes[0];

			// Move the icon form
			if ( this._dragCursorType == DragCursorType.DragIcon )
			{
				this._formDrag.Location = new Point(e.X+5,e.Y -5);
				this._formDrag.Visible = true;
			}
			
			// Remember the target node, so we can set it back
			this._previousNode = treeNode;
		}
	
		/// <summary>
		/// 
		/// </summary>
		/// <param name="e"></param>
		protected override void OnDragLeave(EventArgs e)
		{
			if ( this._selectedNode != null )
			{
				this.SelectedNode = this._selectedNode;
			}

			if ( this._previousNode != null )
			{
				this._previousNode.BackColor = this._dragOverNodeBackColor;
				this._previousNode.ForeColor = this._dragOverNodeForeColor;
			}

			this._formDrag.Visible = false;
			this.Cursor = Cursors.Default;

			// Call cancel event
			if ( this.DragCancel != null )
			{
				DragItemEventArgs ea = new DragItemEventArgs();
				ea.Node = this._selectedNode;

				this.DragCancel(this,ea);
			}
		}
	
		/// <summary>
		/// 
		/// </summary>
		/// <param name="e"></param>
		protected override void OnDragEnter(System.Windows.Forms.DragEventArgs e)
		{
			e.Effect = this._dragMode;

			// Reset the previous node var
			this._previousNode = null;
			this._selectedNode = null;
		}
	
		/// <summary>
		/// 
		/// </summary>
		/// <param name="e"></param>
		protected override void OnDragDrop(System.Windows.Forms.DragEventArgs e)
		{
			// Custom cursor handling
			if ( this._dragCursorType == DragCursorType.DragIcon )
			{
				this.Cursor = Cursors.Default;
			}

			this._formDrag.Visible = false;

			// Check it's a treenode being dragged
			if( e.Data.GetDataPresent("System.Windows.Forms.TreeNode", false) )
			{
				TreeNode dragNode = (TreeNode)e.Data.GetData("System.Windows.Forms.TreeNode");

				TreeNode targetNode = this.Nodes[0];
				//dragNode.Remove();

				this.Cursor = Cursors.Default;

				// Call drag complete event
				if ( this.DragComplete != null )
				{
					DragCompleteEventArgs ea = new DragCompleteEventArgs();
					ea.SourceNode = dragNode;
					ea.TargetNode = targetNode;

					this.DragComplete(this,ea);
				}
			}	
		}

		/// <summary>
		/// 
		/// </summary>
		/// <param name="e"></param>
		protected override void OnKeyUp(KeyEventArgs e)
		{
			if ( e.KeyCode == Keys.Escape )
			{
				if ( this._selectedNode != null )
				{
					this.SelectedNode = this._selectedNode;
				}

				if ( this._previousNode != null )
				{
					this._previousNode.BackColor = SystemColors.HighlightText;
					this._previousNode.ForeColor = SystemColors.ControlText;
				}

				this.Cursor = Cursors.Default;
				this._formDrag.Visible = false;

				// Call cancel event
				if ( this.DragCancel != null )
				{
					DragItemEventArgs ea = new DragItemEventArgs();
					ea.Node = this._selectedNode;

					this.DragCancel(this,ea);
				}
			}
		}

		// Custom double buffering courtesy of http://www.bobpowell.net/tipstricks.htm
		// (doesn't seem to work with this treeview, the wndproc method is the only solution)
		//	protected override void onpaint(painteventargs e) 
		//	{ 
		//		if(this._backbuffer==null) 
		//		{ 
		//			this._backbuffer=new bitmap(this.clientsize.width,this.clientsize.height); 
		//		} 
		//		graphics g=graphics.fromimage(this._backbuffer); 
		//    
		//		//paint your graphics on g here 
		//		g.dispose(); 
		//
		//		//copy the back buffer to the screen 
		//		e.graphics.drawimageunscaled(this._backbuffer,0,0); 
		//
		//		base.onpaint (e); //optional but not recommended 
		//	}
		//
		//	protected override void onpaintbackground(painteventargs e) 
		//	{ 
		//		//don't allow the background to paint
		//	} 
		//
		//	protected override void onsizechanged(eventargs e) 
		//	{ 
		//		if(this._backbuffer!=null) 
		//		{ 
		//			this._backbuffer.dispose(); 
		//			this._backbuffer=null; 
		//		} 
		//		base.onsizechanged (e); 
		//	} 
		#endregion

		#region FormDrag form
		internal class FormDrag : System.Windows.Forms.Form
		{
			#region Components
			public System.Windows.Forms.Label labelText;
			public System.Windows.Forms.PictureBox pictureBox1;
			public System.Windows.Forms.ImageList imageList1;
			private System.ComponentModel.Container components = null;
			#endregion

			#region Constructor, dispose
			public FormDrag()
			{
				InitializeComponent();
			}

			/// <summary>
			/// Clean up any resources being used.
			/// </summary>
			protected override void Dispose( bool disposing )
			{
				if( disposing )
				{
					if(components != null)
					{
						components.Dispose();
					}
				}
				base.Dispose( disposing );
			}
			#endregion

			#region Windows Form Designer generated code
			/// <summary>
			/// Required method for Designer support - do not modify
			/// the contents of this method with the code editor.
			/// </summary>
			private void InitializeComponent()
			{
				this.components = new System.ComponentModel.Container();
				this.labelText = new System.Windows.Forms.Label();
				this.pictureBox1 = new System.Windows.Forms.PictureBox();
				this.imageList1 = new System.Windows.Forms.ImageList(this.components);
				this.SuspendLayout();
				// 
				// labelText
				// 
				this.labelText.BackColor = System.Drawing.Color.Transparent;
				this.labelText.Location = new System.Drawing.Point(16, 2);
				this.labelText.Name = "labelText";
				this.labelText.Size = new System.Drawing.Size(100, 16);
				this.labelText.TabIndex = 0;
				// 
				// pictureBox1
				// 
				this.pictureBox1.Location = new System.Drawing.Point(0, 0);
				this.pictureBox1.Name = "pictureBox1";
				this.pictureBox1.Size = new System.Drawing.Size(16, 16);
				this.pictureBox1.TabIndex = 1;
				this.pictureBox1.TabStop = false;
				// 
				// Form2
				// 
				this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
				this.BackColor = System.Drawing.SystemColors.Control;
				this.ClientSize = new System.Drawing.Size(100, 16);
				this.Controls.Add(this.pictureBox1);
				this.Controls.Add(this.labelText);
				this.Size = new Size(300,500);
				this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.None;
				this.Opacity = 0.3;
				this.ShowInTaskbar = false;
				this.ResumeLayout(false);

			}
			#endregion
		}
		#endregion
	}
	#endregion

	#region DragCursorType enum
	[Serializable]
	public enum DragCursorType
	{
		None,
		DragIcon,
		Custom
	}
	#endregion

	#region Event classes/delegates
	public delegate void DragCompleteEventHandler(object sender,DragCompleteEventArgs e);
	public delegate void DragItemEventHandler(object sender,DragItemEventArgs e);

	public class DragCompleteEventArgs : EventArgs
	{
		/// <summary>
		/// The node that was being dragged
		/// </summary>
		public TreeNode SourceNode
		{
			get
			{
				return this._sourceNode;
			}
			set
			{
				this._sourceNode = value;
			}
		}

		/// <summary>
		/// The node that the source node was dragged onto.
		/// </summary>
		public TreeNode TargetNode
		{
			get
			{
				return this._targetNode;
			}
			set
			{
				this._targetNode = value;
			}
		}
		
		private TreeNode _targetNode;		
		private TreeNode _sourceNode;
	}

	public class DragItemEventArgs : EventArgs
	{
		/// <summary>
		/// The ndoe that was being dragged
		/// </summary>
		public TreeNode Node
		{
			get
			{
				return this._node;
			}
			set
			{
				this._node = value;
			}
		}
		
		private TreeNode _node;
	}
	#endregion
}
