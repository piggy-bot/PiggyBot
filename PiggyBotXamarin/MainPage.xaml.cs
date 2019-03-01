using Newtonsoft.Json.Linq;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Net;
using System.Net.Http;
using System.Reflection;
using System.Text;
using System.Threading.Tasks;
using Xamarin.Forms;
using Xamarin.Forms.Internals;

namespace PiggyBotXamarin
{
    public partial class MainPage : ContentPage
    {
        private static readonly HttpClient client = new HttpClient();
        public MainPage()
        {
            InitializeComponent();
        }

        private void signIn(object sender, EventArgs e)
        {
            var piggy = new Piggy(piggyId.Text);
            GetRequest("https://piggylistener.azurewebsites.net/api/doesExist?code=Ma29gV1Q6hlhIdsK0PTMZLzqvpkhAvtZkSq5cDlaKpkxosFiRT1DWA==&pigName="+piggyId.Text, piggy);
        }

        async void GetRequest(string url, Piggy piggy)
        { 
            HttpClient client = new HttpClient();
            HttpResponseMessage getPigResponse = await client.GetAsync(url);

            HttpContent getPigContent = getPigResponse.Content;
            string pigName = await getPigContent.ReadAsStringAsync();
            JObject pigNameJson = JObject.Parse(pigName);
            if (!"No results were found. SOME ANOTHER MSG".Equals(pigNameJson["m_StringValue"].ToString()))
            {
                HttpResponseMessage getMoneyResponse = await client.GetAsync("https://piggylistener.azurewebsites.net/api/getGoal?code=JaeJjoWnQHo3NDxXaYk/JRBCnYWpaTmxjNBn5r0Wsk/griAzthN/0Q==&pigName=" + piggy.id);
                HttpContent getMoneyContent = getMoneyResponse.Content;
                string goalString = await getMoneyContent.ReadAsStringAsync();
                double goal = Convert.ToDouble(goalString);
                if (-1 == goal)
                {
                    await Navigation.PushAsync(new saveAmountPage(piggy));
                }
                else
                {
                    piggy.saving_goal = goal;
                    System.Console.WriteLine("piggy with a saved goal " + piggy.id + " " + piggy.saving_goal);
                    HttpResponseMessage getSavedResponse = await client.GetAsync("https://piggylistener.azurewebsites.net/api/GetMoney?code=zOb1CdXaa6i1qhO2agOPVrxnkaMg0nQODlVPN1KyQ0FXPvpHNbH1Xw==&pigName=" + piggy.id);
                    HttpContent getSavedContent = getSavedResponse.Content;
                    string savedString = await getSavedContent.ReadAsStringAsync();
                    piggy.saved_amount = Convert.ToDouble(savedString);
                    await Navigation.PushAsync(new MainPigPage(piggy));
                }
            }
            else
            {
                await DisplayAlert("Alert", "Piggy doesn't exist, enter a valid id", "OK");
            }
            
        }
    }
}
