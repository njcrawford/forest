using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace ConfigurationGUI
{
    public partial class frmScheduleTask : Form
    {
        public frmScheduleTask()
        {
            InitializeComponent();
        }

        private void btnCancel_Click(object sender, EventArgs e)
        {
            this.Close();
        }

        private void btnOk_Click(object sender, EventArgs e)
        {
            // Create or modify the scheduled task
            MessageBox.Show("Not yet implemented");
        }
    }
}
