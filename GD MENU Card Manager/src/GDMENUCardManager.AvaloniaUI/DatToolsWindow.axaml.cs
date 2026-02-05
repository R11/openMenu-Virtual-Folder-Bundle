using Avalonia.Controls;
using Avalonia.Interactivity;
using Avalonia.Markup.Xaml;
using Avalonia.Threading;
using MessageBox.Avalonia;
using MessageBox.Avalonia.Models;
using System;
using System.IO;
using System.Threading.Tasks;
using GDMENUCardManager.Core;

namespace GDMENUCardManager
{
    public partial class DatToolsWindow : Window
    {
        private readonly Core.Manager _manager;
        private readonly Func<Task> _reloadCallback;

        private string _importSourcePath;
        private string _exportTargetPath;

        private const int MaxPathDisplayLength = 50;

        // UI controls
        private TextBlock TextImportSourcePath;
        private TextBlock TextExportTargetPath;
        private Button ButtonBeginImport;
        private Button ButtonBeginExport;
        private RadioButton RadioImportMissing;
        private RadioButton RadioImportAll;

        public DatToolsWindow()
        {
            InitializeComponent();
        }

        public DatToolsWindow(Core.Manager manager, Func<Task> reloadCallback)
        {
            InitializeComponent();

            _manager = manager;
            _reloadCallback = reloadCallback;

            // Get references to UI controls
            TextImportSourcePath = this.FindControl<TextBlock>("TextImportSourcePath");
            TextExportTargetPath = this.FindControl<TextBlock>("TextExportTargetPath");
            ButtonBeginImport = this.FindControl<Button>("ButtonBeginImport");
            ButtonBeginExport = this.FindControl<Button>("ButtonBeginExport");
            RadioImportMissing = this.FindControl<RadioButton>("RadioImportMissing");
            RadioImportAll = this.FindControl<RadioButton>("RadioImportAll");

            this.KeyUp += (s, e) => { if (e.Key == Avalonia.Input.Key.Escape) Close(); };
        }

        private void InitializeComponent()
        {
            AvaloniaXamlLoader.Load(this);
        }

        /// <summary>
        /// Truncate a path for display, adding "..." if too long.
        /// </summary>
        private string TruncatePath(string path)
        {
            if (string.IsNullOrEmpty(path))
                return "[no folder selected]";

            if (path.Length <= MaxPathDisplayLength)
                return path;

            // Show beginning and end with ... in middle
            int halfLength = (MaxPathDisplayLength - 3) / 2;
            return path.Substring(0, halfLength) + "..." + path.Substring(path.Length - halfLength);
        }

        #region Import Tab

        private async void ChooseImportFolder_Click(object sender, RoutedEventArgs e)
        {
            var dialog = new OpenFolderDialog
            {
                Title = "Select DAT import folder"
            };

            var result = await dialog.ShowAsync(this);
            if (!string.IsNullOrEmpty(result))
            {
                // Validate the folder contains at least one DAT file
                var boxPath = Path.Combine(result, "BOX.DAT");
                var metaPath = Path.Combine(result, "META.DAT");

                if (!File.Exists(boxPath) && !File.Exists(metaPath))
                {
                    await ShowError("Invalid Folder", "Selected folder does not contain BOX.DAT or META.DAT.");
                    return;
                }

                _importSourcePath = result;
                TextImportSourcePath.Text = TruncatePath(result);
                TextImportSourcePath.Foreground = Avalonia.Media.Brushes.Black;
                ButtonBeginImport.IsEnabled = true;
            }
        }

        private async void BeginImport_Click(object sender, RoutedEventArgs e)
        {
            if (string.IsNullOrEmpty(_importSourcePath))
                return;

            // Confirmation dialog
            var confirmResult = await MessageBoxManager.GetMessageBoxCustomWindow(new MessageBox.Avalonia.DTO.MessageBoxCustomParams
            {
                ContentTitle = "Confirm Import",
                ContentMessage = "This will backup current DAT files and merge entries from the selected folder.\n\nContinue?",
                Icon = MessageBox.Avalonia.Enums.Icon.Warning,
                ShowInCenter = true,
                WindowStartupLocation = WindowStartupLocation.CenterOwner,
                ButtonDefinitions = new ButtonDefinition[]
                {
                    new ButtonDefinition { Name = "Continue" },
                    new ButtonDefinition { Name = "Cancel" }
                }
            }).ShowDialog(this);

            if (confirmResult != "Continue")
                return;

            bool overwriteExisting = RadioImportAll?.IsChecked == true;

            // Show progress window
            var progressWindow = new ProgressWindow();
            progressWindow.Title = "Importing DAT Entries";
            progressWindow.TextContent = "Importing...";
            progressWindow.TotalItems = 100;
            progressWindow.ProcessedItems = 0;

            _ = progressWindow.ShowDialog(this);

            try
            {
                var result = await Task.Run(() =>
                {
                    return _manager.ImportDatEntries(_importSourcePath, overwriteExisting, progress =>
                    {
                        Dispatcher.UIThread.Post(() =>
                        {
                            progressWindow.ProcessedItems = (int)(progress * 100);
                        });
                    });
                });

                progressWindow.AllowClose();
                progressWindow.Close();

                if (!result.success)
                {
                    await ShowError("Import Failed", result.errorMessage);
                    return;
                }

                // Show success message first
                var message = $"Import completed successfully.\n\nBOX.DAT entries merged: {result.boxEntriesMerged}\nMETA.DAT entries merged: {result.metaEntriesMerged}";
                if (result.boxEntriesMerged > 0)
                {
                    message += "\n\nICON.DAT was automatically regenerated using the updated contents of BOX.DAT.";
                }
                await ShowInfo("Import Complete", message);

                // Close this window and reload
                this.Close();

                if (_reloadCallback != null)
                {
                    await _reloadCallback();
                }
            }
            catch (Exception ex)
            {
                progressWindow.AllowClose();
                progressWindow.Close();
                await ShowError("Import Failed", $"An error occurred: {ex.Message}");
            }
        }

        #endregion

        #region Export Tab

        private async void ChooseExportFolder_Click(object sender, RoutedEventArgs e)
        {
            var dialog = new OpenFolderDialog
            {
                Title = "Select PNG export folder"
            };

            var result = await dialog.ShowAsync(this);
            if (!string.IsNullOrEmpty(result))
            {
                _exportTargetPath = result;
                TextExportTargetPath.Text = TruncatePath(result);
                TextExportTargetPath.Foreground = Avalonia.Media.Brushes.Black;
                ButtonBeginExport.IsEnabled = true;
            }
        }

        private async void BeginExport_Click(object sender, RoutedEventArgs e)
        {
            if (string.IsNullOrEmpty(_exportTargetPath))
                return;

            // Show progress window
            var progressWindow = new ProgressWindow();
            progressWindow.Title = "Exporting Artwork";
            progressWindow.TextContent = "Exporting...";
            progressWindow.TotalItems = 100;
            progressWindow.ProcessedItems = 0;

            _ = progressWindow.ShowDialog(this);

            try
            {
                var result = await Task.Run(() =>
                {
                    return _manager.ExportArtworkToPngs(_exportTargetPath, progress =>
                    {
                        Dispatcher.UIThread.Post(() =>
                        {
                            progressWindow.ProcessedItems = (int)(progress * 100);
                        });
                    });
                });

                progressWindow.AllowClose();
                progressWindow.Close();

                if (!result.success)
                {
                    await ShowError("Export Failed", result.errorMessage);
                    return;
                }

                // Keep window open, just show success
                await ShowInfo("Export Complete", $"Exported {result.exportedCount} artwork file(s) to PNG.");
            }
            catch (Exception ex)
            {
                progressWindow.AllowClose();
                progressWindow.Close();
                await ShowError("Export Failed", $"An error occurred: {ex.Message}");
            }
        }

        #endregion

        #region Clear Tab

        private async void ClearDats_Click(object sender, RoutedEventArgs e)
        {
            // Confirmation dialog
            var confirmResult = await MessageBoxManager.GetMessageBoxCustomWindow(new MessageBox.Avalonia.DTO.MessageBoxCustomParams
            {
                ContentTitle = "Confirm Clear",
                ContentMessage = "This will backup current DAT files and then clear ALL artwork and metadata entries.\n\nThis action cannot be undone. Continue?",
                Icon = MessageBox.Avalonia.Enums.Icon.Warning,
                ShowInCenter = true,
                WindowStartupLocation = WindowStartupLocation.CenterOwner,
                ButtonDefinitions = new ButtonDefinition[]
                {
                    new ButtonDefinition { Name = "Clear All" },
                    new ButtonDefinition { Name = "Cancel" }
                }
            }).ShowDialog(this);

            if (confirmResult != "Clear All")
                return;

            // Show progress window
            var progressWindow = new ProgressWindow();
            progressWindow.Title = "Clearing DAT Files";
            progressWindow.TextContent = "Clearing...";
            progressWindow.TotalItems = 100;
            progressWindow.ProcessedItems = 50; // Show some progress for indeterminate

            _ = progressWindow.ShowDialog(this);

            try
            {
                var result = await Task.Run(() => _manager.ClearAllDatEntries());

                progressWindow.AllowClose();
                progressWindow.Close();

                if (!result.success)
                {
                    await ShowError("Clear Failed", result.errorMessage);
                    return;
                }

                // Show success message first
                await ShowInfo("Clear Complete", "All DAT entries have been cleared.");

                // Close this window and reload
                this.Close();

                if (_reloadCallback != null)
                {
                    await _reloadCallback();
                }
            }
            catch (Exception ex)
            {
                progressWindow.AllowClose();
                progressWindow.Close();
                await ShowError("Clear Failed", $"An error occurred: {ex.Message}");
            }
        }

        #endregion

        #region Helpers

        private async Task ShowError(string title, string message)
        {
            await MessageBoxManager.GetMessageBoxStandardWindow(title, message,
                MessageBox.Avalonia.Enums.ButtonEnum.Ok, MessageBox.Avalonia.Enums.Icon.Error)
                .ShowDialog(this);
        }

        private async Task ShowInfo(string title, string message)
        {
            await MessageBoxManager.GetMessageBoxStandardWindow(title, message,
                MessageBox.Avalonia.Enums.ButtonEnum.Ok, MessageBox.Avalonia.Enums.Icon.Info)
                .ShowDialog(this);
        }

        #endregion
    }
}
