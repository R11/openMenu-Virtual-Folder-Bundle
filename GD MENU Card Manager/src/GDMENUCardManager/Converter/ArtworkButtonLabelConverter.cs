using System;
using System.Globalization;
using System.Windows.Data;

namespace GDMENUCardManager.Converter
{
    public class ArtworkButtonLabelConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            if (value is bool hasArtwork)
            {
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
