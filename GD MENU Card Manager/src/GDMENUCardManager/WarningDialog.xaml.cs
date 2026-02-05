using System.Windows;

namespace GDMENUCardManager
{
    public partial class WarningDialog : Window
    {
        public string Message { get; set; }
        public bool Proceed { get; private set; }

        public WarningDialog(string message)
        {
            InitializeComponent();
            Message = message;
            DataContext = this;
        }

        private void ReturnButton_Click(object sender, RoutedEventArgs e)
        {
            Proceed = false;
            DialogResult = false;
        }

        private void ProceedButton_Click(object sender, RoutedEventArgs e)
        {
            Proceed = true;
            DialogResult = true;
        }
    }
}
