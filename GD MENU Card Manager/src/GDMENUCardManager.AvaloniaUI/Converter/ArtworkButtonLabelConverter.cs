using Avalonia.Data.Converters;
using System;
using System.Globalization;

namespace GDMENUCardManager.Converter
{
    public class ArtworkButtonLabelConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            if (value is bool hasArtwork)
            {
                if (parameter is string param && param == "tooltip")
                    return hasArtwork ? "Edit currently assigned artwork" : "Assign artwork";
                return hasArtwork ? "Manage" : "Assign";
            }
            return "Assign";
        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            throw new NotImplementedException();
        }
    }
}
