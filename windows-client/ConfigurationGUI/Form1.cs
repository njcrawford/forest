using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

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
                txtServerURL.Text = "http://url-not-set/forest/";
            }
            else
            {
                txtServerURL.Text = settings.getValue("server_url");
            }
        }

        private void btnOK_Click(object sender, EventArgs e)
        {
            if (txtServerURL.Text.StartsWith("http") && txtServerURL.Text.EndsWith("/"))
            {
                settings.setValue("server_url", txtServerURL.Text);
                settings.save();
            }
            else
            {
                MessageBox.Show("Server URL must start with 'http' and end with '/'");
            }
        }

        private void btnClose_Click(object sender, EventArgs e)
        {
            Application.Exit();
        }
    }
}