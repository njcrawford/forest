using Microsoft.Win32.TaskScheduler;
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

            // Get the service on the local machine
            using (TaskService ts = new TaskService())
            {
                // FIgure out what version of task scheduler we're using
                bool isVersionOne = ts.HighestSupportedVersion < new Version(1, 2);

                // Create a new task definition and assign properties
                TaskDefinition td = ts.NewTask();
                td.RegistrationInfo.Description = "Runs Forest Client daily";

                // Create a trigger that will fire the task at this time every day
                td.Triggers.Add(new DailyTrigger { DaysInterval = 1, StartBoundary = dateTimePicker1.Value });

                // Create an action that will launch forest-client whenever the trigger fires
                string forestClientPath = System.IO.Path.Combine(Environment.CurrentDirectory, "forest-client.exe");
                td.Actions.Add(new ExecAction(forestClientPath, null, null));

                // Change some settings of the task to fit our application.
                // It may make sense to break some of these settings out as controls the user can adjust.
                td.Settings.RunOnlyIfNetworkAvailable = true;

                // Don't stop just because someone started using the system
                td.Settings.IdleSettings.StopOnIdleEnd = false;

                // Run even if system is not idle
                td.Settings.RunOnlyIfIdle = false;

                // Allow the app to run for up to 6 hours. This should be enough
                // to install updates, if/when we're ever able to do so on Windows.
                td.Settings.ExecutionTimeLimit = TimeSpan.FromHours(6);

                // Some things arent compatible with older versions of task scheduler
                if (!isVersionOne)
                {
                    // Stop and existing instances when starting a new one. This
                    // shouldn't ever happen, as far as I can tell.
                    td.Settings.MultipleInstances = TaskInstancesPolicy.StopExisting;

                    // If the scheduled time is missed, start as soon as the system is available
                    td.Settings.StartWhenAvailable = true;
                }

                // Register the task in the root folder
                ts.RootFolder.RegisterTaskDefinition(@"Forest Client", td,
                    TaskCreation.CreateOrUpdate, "SYSTEM", null,
                    TaskLogonType.ServiceAccount);

                // In case we ever need to remove the task
                // Remove the task we just created
                //ts.RootFolder.DeleteTask("Test");
            }

            //MessageBox.Show("Finished!");
            this.Close();
        }

        private void frmScheduleTask_Load(object sender, EventArgs e)
        {
            // Limit the date range to today
            dateTimePicker1.MinDate = DateTime.Today;
            dateTimePicker1.MaxDate = DateTime.Today + TimeSpan.FromDays(1);

            // Default to 3:05 AM
            DateTime defaultRunTime = DateTime.Today + new TimeSpan(3, 5, 0);
            dateTimePicker1.Value = defaultRunTime;
        }
    }
}
