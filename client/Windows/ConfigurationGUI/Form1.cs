using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Security.Principal;
using System.Diagnostics;

namespace ConfigurationGUI
{
    public partial class frmMain : Form
    {
        public frmMain()
        {
            InitializeComponent();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            frmSettings settings = new frmSettings();
            settings.ShowDialog();
        }

        private void button2_Click(object sender, EventArgs e)
        {
            frmScheduleTask task = new frmScheduleTask();
            task.ShowDialog();
        }

        private void btnRun_Click(object sender, EventArgs e)
        {
            string prevText = btnRun.Text;
            btnRun.Text = "Running...";
            btnRun.Enabled = false;

            Process forestClient = new Process();
            forestClient.StartInfo.FileName = "forest-client.exe";
            forestClient.StartInfo.UseShellExecute = false;
            //forestClient.StartInfo.RedirectStandardOutput = true;
            //forestClient.StartInfo.RedirectStandardError = true;
            //forestClient.StartInfo.CreateNoWindow = true;
            forestClient.StartInfo.Arguments = "--wait";
            if (chkVerbose.Checked)
            {
                forestClient.StartInfo.Arguments += " --verbose";
            }
            forestClient.Start();

            //frmTextbox output = new frmTextbox();
            //output.Text = "Forest Client run results";
            //output.setTextboxText(forestClient.StandardOutput.ReadToEnd() + Environment.NewLine + forestClient.StandardError.ReadToEnd());
            forestClient.WaitForExit();
            //output.ShowDialog();
            btnRun.Text = prevText;
            btnRun.Enabled = true;
        }
    }
}