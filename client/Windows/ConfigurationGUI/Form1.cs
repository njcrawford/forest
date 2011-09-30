using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Security.Principal;

namespace ConfigurationGUI
{
    public partial class frmMain : Form
    {
        private NJCrawford.IniFile settings;

        public frmMain()
        {
            InitializeComponent();
        }

        private void frmMain_Load(object sender, EventArgs e)
        {
            settings = new NJCrawford.IniFile("forest-client.conf");
            // set a default for server url if there is nothing set
            if (settings.getValue("server_url") == null)
            {
                txtServerURL.Text = "http://url-not-set/forest";
            }
            else
            {
                string removeQuotes = settings.getValue("server_url");
                txtServerURL.Text = removeQuotes.Replace("\"", "");
            }
        }

        private void btnOK_Click(object sender, EventArgs e)
        {
            string testURL = txtServerURL.Text.Trim();
            if (testURL.StartsWith("http"))
            {
                settings.setValue("server_url", "\"" + testURL + "\"");
                settings.save();
                Application.Exit();
            }
            else
            {
                MessageBox.Show("Server URL must start with 'http'.");
            }
        }

        private void btnClose_Click(object sender, EventArgs e)
        {
            Application.Exit();
        }
    }
}