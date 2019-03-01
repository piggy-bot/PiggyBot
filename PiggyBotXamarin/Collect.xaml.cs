using Newtonsoft.Json.Linq;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Net;
using System.Net.Http;
using System.Text;
using System.Threading.Tasks;

using Xamarin.Forms;
using Xamarin.Forms.Xaml;

namespace PiggyBotXamarin
{
    [XamlCompilation(XamlCompilationOptions.Compile)]
    public partial class Collect : ContentPage
    {
        public Piggy piggy;
        public Collect(Piggy piggy)
        {
            this.piggy = piggy;
            InitializeComponent();
            saved.Text = this.piggy.saved_amount + "/" + this.piggy.saving_goal;
        }

        private void collectMoney(object sender, EventArgs e)
        {
            this.piggy.saved_amount = 0;
            this.piggy.saving_goal = 0;
            string url = "https://piggylistener.azurewebsites.net/api/openPig?code=idkXIO/5wYUsztEZa1h4wP52FsusQsgHxthTu1i2XfQITP4ZAerAjQ==&pigName="+this.piggy.id+"&password="+piggyPassword.Text;
            GetRequest(url);
        }

        async void GetRequest(string url)
        {
            HttpClient client = new HttpClient();
            HttpResponseMessage getPigResponse = await client.GetAsync(url);

            HttpContent getPigContent = getPigResponse.Content;
            string collectResult = await getPigContent.ReadAsStringAsync();
            // TODO check for status 200
            JObject pigNameJson = JObject.Parse(collectResult);
            System.Console.WriteLine(pigNameJson["m_StringValue"]);
            await Navigation.PushAsync(new saveAmountPage(this.piggy));
        }
    }
}