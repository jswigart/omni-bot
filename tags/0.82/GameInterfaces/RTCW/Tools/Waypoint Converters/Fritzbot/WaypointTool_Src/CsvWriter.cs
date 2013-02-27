using System;
using System.Collections;
using System.IO;
using System.Text;

namespace WaypointTool
{
    public class CsvWriter
    {
        private ArrayList _columns = new ArrayList();
        private ArrayList _rows = new ArrayList();

        public ArrayList Columns
        {
            get { return _columns; }
        }

        public ArrayList Rows
        {
            get { return _rows; }
        }

        public void AddRow(ArrayList data)
        {
            _rows.Add(data);
        }

        public void Write(string fileName)
        {
            if (File.Exists(fileName))
            {
                File.Delete(fileName);
            }

            StringBuilder buff = new StringBuilder();

            foreach (String val in _columns)
            {
                buff.Append(Escape(val));
                buff.Append(",");
            }

            buff.Remove(buff.Length - 1, 1);

            buff.Append("\r\n");

            foreach (ArrayList row in _rows)
            {
                foreach (String val in row)
                {
                    buff.Append(Escape(val));
                    buff.Append(",");
                }

                buff.Remove(buff.Length - 1, 1);

                buff.Append("\r\n");
            }

            TextWriter writer = null;

            try
            {
                writer = new StreamWriter(fileName);
                writer.Write(buff.ToString());
            }
            finally
            {
                if (writer != null)
                {
                    writer.Close();
                }
            }
        }

        private string Escape(string val)
        {
            val = val.Replace("\"", "'");
            val = "\"" + val + "\"";
            val = val.Replace(",", "\",\"");
            return val;
        }
    }
}
