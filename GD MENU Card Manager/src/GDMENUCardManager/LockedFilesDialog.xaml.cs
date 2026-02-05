using System.Collections.Generic;
using System.Linq;
using System.Windows;

namespace GDMENUCardManager
{
    public partial class LockedFilesDialog : Window
    {
        public class LockedFileInfo
        {
            public string Path { get; set; }
            public string Error { get; set; }
        }

        public LockedFilesDialog(Dictionary<string, string> lockedFiles)
        {
            InitializeComponent();

            var items = lockedFiles.Select(kvp => new LockedFileInfo
            {
                Path = kvp.Key,
                Error = kvp.Value
            }).ToList();

            FileListBox.ItemsSource = items;
        }

        private void RetryButton_Click(object sender, RoutedEventArgs e)
        {
            DialogResult = true;
            Close();
        }

        private void CancelButton_Click(object sender, RoutedEventArgs e)
        {
            DialogResult = false;
            Close();
        }
    }
}
