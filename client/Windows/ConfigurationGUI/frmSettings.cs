using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace ConfigurationGUI
{
    public partial class frmSettings : Form
    {
        private NJCrawford.IniFile settings;

        public frmSettings()
        {
            InitializeComponent();
        }

        private void frmSettings_Load(object sender, EventArgs e)
        {
            settings = new NJCrawford.IniFile("forest-client.conf");
            // set a default for server url if there is nothing set
            String serverUrl = settings.getValue("server_url", "");
            if (String.IsNullOrEmpty(serverUrl))
            {
                txtServerURL.Text = "http://url-not-set/forest";
            }
            else
            {
                txtServerURL.Text = serverUrl.Replace("\"", "");
            }
        }

        private void btnOK_Click(object sender, EventArgs e)
        {
            string testURL = txtServerURL.Text.Trim();
            if (testURL.StartsWith("http"))
            {
                settings.setValue("server_url", "\"" + testURL + "\"");
                settings.save();
                this.Close();
            }
            else
            {
                MessageBox.Show("Server URL must start with 'http'.");
            }
        }

        private void btnClose_Click(object sender, EventArgs e)
        {
            this.Close();
        }
    }
}
