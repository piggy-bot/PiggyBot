using Newtonsoft.Json.Linq;
using System;
using System.Net;
using System.Net.Http;
using Xamarin.Forms;
using Xamarin.Forms.Xaml;

namespace PiggyBotXamarin
{
    [XamlCompilation(XamlCompilationOptions.Compile)]
    public partial class saveAmountPage : ContentPage
    {
        public Piggy piggy;
        public saveAmountPage(Piggy piggy)
        {
            this.piggy = piggy;
            InitializeComponent();
        }

        private void saveAmount(object sender, EventArgs e)
        {
            this.piggy.saving_goal = Convert.ToDouble(savingamount.Text);
            saved.Text = "0/" + this.piggy.saving_goal;
            GetRequest("https://piggylistener.azurewebsites.net/api/SetMoneyGoal?code=RjyVMTMC/HQz5VTa88m8FX3iMvYnSoClIONjl7pE1oyeAQRwOnanLw==&pigName="+piggy.id+"&moneygoal="+savingamount.Text);
        }


        async void GetRequest(string url)
        {
            HttpClient client = new HttpClient();
            HttpResponseMessage getPigResponse = await client.GetAsync(url);
            if (getPigResponse.StatusCode == HttpStatusCode.OK)
            {
                await Navigation.PushAsync(new MainPigPage(this.piggy));
            }

        }
    }
}
