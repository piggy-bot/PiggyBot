using System;
using System.Collections.Generic;
using System.Text;

namespace PiggyBotXamarin
{
    public class Piggy
    {
        public string id { get; set; }
        public string password { get; set; }
        public double saving_goal { get; set; }
        public double saved_amount { get; set; }
        public DateTime last_saved { get; set; }

        public Piggy(string id)
        {
            this.id = id;
            this.password = "";
            this.saving_goal = 0;
            this.saved_amount = 0;
            this.last_saved = DateTime.Now;
        }
    }
}
