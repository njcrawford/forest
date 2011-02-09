using System;
using System.Collections.Generic;
using System.Text;
using WUApiLib;
using System.Management;
using System.Net;
using System.IO;

namespace forest_client
{
    class Program
    {
        const int rpcVersion = 2;

        static List<string> updates = new List<string>();
        static NJCrawford.IniFile settings = new NJCrawford.IniFile("forest-client.conf");

        static void getAvailableUpdates()
        {
            UpdateSessionClass uSession = new UpdateSessionClass();
            IUpdateSearcher uSearcher = uSession.CreateUpdateSearcher();
            ISearchResult uResult = uSearcher.Search("IsInstalled=0 and Type='Software'");
            foreach (IUpdate update in uResult.Updates)
            {
                if (update.AutoSelectOnWebSites)
                {
                    updates.Add(update.Title);
                }
            }
        }

        static void contactServer()
        {
            // Create a request using a URL that can receive a post. 
            WebRequest request = WebRequest.Create(settings.getValue("server_url") + "collect.php");
            // Set the Method property of the request to POST.
            request.Method = "POST";
            // Create POST data and convert it to a byte array.
            string postData = "rpc_version=" + rpcVersion;
            postData += "&system_name=" + System.Environment.MachineName;
            postData += "&available_updates=";
            bool isFirst = true;
            foreach (string s in updates)
            {
                if (isFirst)
                {
                    isFirst = false;
                }
                else
                {
                    postData += ",";
                }
                postData += s;
            }
            byte[] byteArray = Encoding.UTF8.GetBytes(postData);
            // Set the ContentType property of the WebRequest.
            request.ContentType = "application/x-www-form-urlencoded";
            // Set the ContentLength property of the WebRequest.
            request.ContentLength = byteArray.Length;
            // Get the request stream.
            Stream dataStream = request.GetRequestStream();
            // Write the data to the request stream.
            dataStream.Write(byteArray, 0, byteArray.Length);
            // Close the Stream object.
            dataStream.Close();
            // Get the response.
            WebResponse response = request.GetResponse();
            // Display the status.
            Console.WriteLine(((HttpWebResponse)response).StatusDescription);
            // Get the stream containing content returned by the server.
            dataStream = response.GetResponseStream();
            // Open the stream using a StreamReader for easy access.
            StreamReader reader = new StreamReader(dataStream);
            // Read the content.
            string responseFromServer = reader.ReadToEnd();
            // Display the content.
            Console.WriteLine(responseFromServer);
            // Clean up the streams.
            reader.Close();
            dataStream.Close();
            response.Close();
        }

        static void Main(string[] args)
        {
            // set a default for server url if there nothing set
            if (settings.getValue("server_url") == null)
            {
                settings.setValue("server_url", "http://url-not-set/forest/");
                // save the file so user can easily edit it
                settings.save();
            }
            Console.WriteLine("Searching for available updates...");
            getAvailableUpdates();
            /*foreach (string s in updates)
            {
                Console.WriteLine(s);
            }*/
            Console.WriteLine("Contacting forest server...");
            contactServer();
            //Console.WriteLine("Press enter to exit");
            //Console.In.ReadLine();
        }
    }
}
