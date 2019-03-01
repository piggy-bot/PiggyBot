using Newtonsoft.Json.Linq;
using System;
using System.Globalization;
using System.Collections.Generic;
using System.Linq;
using System.Net.Http;
using System.Text;
using System.Threading.Tasks;

using Xamarin.Forms;
using Xamarin.Forms.Xaml;

namespace PiggyBotXamarin
{
	[XamlCompilation(XamlCompilationOptions.Compile)]
	public partial class MainPigPage : ContentPage
	{
        public Piggy piggy;
        public MainPigPage (Piggy piggy)
		{
            this.piggy = piggy;
            InitializeComponent ();

            saved.Text = this.piggy.saved_amount + "/" + this.piggy.saving_goal;
            if (this.piggy.saved_amount >= this.piggy.saving_goal && this.piggy.saving_goal != 0)
            {
                saved.GestureRecognizers.Add(new TapGestureRecognizer {
                    Command = new Command(() =>
                    {
                        Navigation.PushAsync(new Collect(this.piggy));
                    }),
                    NumberOfTapsRequired = 1
                });
                pigStatus.Source = "happypig.jpg";
                pigSpeech.Source = "happypigcoinspeech.jpg";
            }
            else 
            {
                GetRequest("https://piggylistener.azurewebsites.net/api/lastDepositDate?code=qmEJW5OeUbET1bjHVVzyD/8aAoFSMGh9BZuIzdt99iDJ8jSH2oEkaA==&pigName=" + this.piggy.id);
            }
        }

        async void GetRequest(string url)
        {
            HttpClient client = new HttpClient();
            HttpResponseMessage getLastSavedResponse = await client.GetAsync(url);

            HttpContent getLastSavedContent = getLastSavedResponse.Content;
            string pigLastSaved = await getLastSavedContent.ReadAsStringAsync();
            JObject pigLastSavedJson = JObject.Parse(pigLastSaved);
            System.Console.WriteLine("last deposit " + pigLastSavedJson["m_StringValue"]);
            if (!"No results were found. SOME ANOTHER MSG".Equals(pigLastSavedJson["m_StringValue"].ToString()))
            {
                this.piggy.last_saved = DateTime.Parse(pigLastSavedJson["m_StringValue"].ToString());
                TimeSpan interval = DateTime.Now - this.piggy.last_saved;
                if(interval.Minutes > 1)
                {
                    pigStatus.Source = "sadpig.jpg";
                    pigSpeech.Source = "sadpigspeech.jpg";
                }
            }
        }

    }
}