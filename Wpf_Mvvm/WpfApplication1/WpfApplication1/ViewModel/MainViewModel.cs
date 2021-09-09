using GalaSoft.MvvmLight;
using GalaSoft.MvvmLight.Command;
using System.Windows;
using System.Text.RegularExpressions;
using System.Collections.Generic;


namespace WpfApplication1.ViewModel
{
    public class person
    {
        public string name { get;  set; }
        public int id { get;  set; }
        public int age { get;  set; }
        public string tel { get;  set; }
        public string address { get;  set; }
        public person(string name_, int id_, int age_, string tel_, string address_)
        {
            name = name_;
            id = id_;
            age = age_;
            tel = tel_;
            address = address_;
        }
        public person()
        { }
    }

    /// <summary>
    /// This class contains properties that the main View can data bind to.
    /// <para>
    /// Use the <strong>mvvminpc</strong> snippet to add bindable properties to this ViewModel.
    /// </para>
    /// <para>
    /// You can also use Blend to data bind with the tool's support.
    /// </para>
    /// <para>
    /// See http://www.galasoft.ch/mvvm
    /// </para>
    /// </summary>
    public class MainViewModel : ViewModelBase
    {
        /// <summary>
        /// Initializes a new instance of the MainViewModel class.
        /// </summary>
        public MainViewModel()
        {
            ////if (IsInDesignMode)
            ////{
            ////    // Code runs in Blend --> create design time data.
            ////}
            ////else
            ////{
            ////    // Code runs "for real"
            ////}
            GridViewSourve = new List<person>();
            GridViewSourve.Add(new person("xui", 1, 24, "13866891234", "shenzhen"));
            GridViewSourve.Add(new person("gong", 1, 24, "16544592235", "wuhan"));
            GridViewSourve.Add(new person("wang", 1, 24, "13866895934", "xinjiang"));
            GridViewSourve.Add(new person("bai", 1, 24, "138665895934", "beijing"));
        }

        private int? _combobox_text;
        public int? combobox_text
        {
            get { return _combobox_text; }
            set
            {
                _combobox_text = value;
                RaisePropertyChanged();
            }
        }

        private RelayCommand<string> _cmd;
        public RelayCommand<string> cmd
        {
            get
            {
                return _cmd ?? (_cmd = new RelayCommand<string>(cmd_func));
            }
        }
        public void cmd_func(string str)
        {
            MessageBox.Show(str);
        }

        RelayCommand btn_clk_;
        public RelayCommand btn_clk
        {
            get
            {
                return btn_clk_ ?? (btn_clk_ = new RelayCommand(BTN2_CLK));
            }
        }

        private void BTN2_CLK()
        {
            string str = "688.5536jSH";
            bool is_match = Regex.IsMatch(str, @"^688.*SH$");
            MessageBox.Show($"is_match: {is_match.ToString()}");
        }

        public List<person> GridViewSourve_;
        public List<person> GridViewSourve
        {
            get { return GridViewSourve_; }
            set
            {
                GridViewSourve_ = value;
                RaisePropertyChanged();
            }
        }

    }
}