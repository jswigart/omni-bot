using System;
using System.Drawing.Printing;

namespace WaypointTool
{
	public interface IPrintableUserControl
	{
		PrintDocument PrintDocument { get; }
	}

    public interface IExportableUserControl
    {
        CsvWriter GetCsvWriter();
    }
}
