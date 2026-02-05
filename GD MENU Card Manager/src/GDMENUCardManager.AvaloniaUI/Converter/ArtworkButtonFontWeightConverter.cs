using Avalonia.Data.Converters;
using Avalonia.Media;
using System;
using System.Globalization;

namespace GDMENUCardManager.Converter
{
    public class ArtworkButtonFontWeightConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            if (value is bool hasArtwork)
            {
                // Bold for "Assign" (no artwork), Normal for "Manage" (has artwork)
                return hasArtwork ? FontWeight.Normal : FontWeight.Bold;
            }
            return FontWeight.Bold;
        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            throw new NotImplementedException();
        }
    }
}
