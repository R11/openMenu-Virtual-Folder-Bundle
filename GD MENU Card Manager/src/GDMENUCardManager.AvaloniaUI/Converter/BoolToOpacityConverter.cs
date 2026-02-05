using Avalonia.Data.Converters;
using System;
using System.Globalization;

namespace GDMENUCardManager.Converter
{
    /// <summary>
    /// Converts bool to opacity (1.0 or 0.0).
    /// Use with IsHitTestVisible binding to simulate WPF's Visibility.Hidden.
    /// </summary>
    public class BoolToOpacityConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            if (value is bool boolValue)
            {
                return boolValue ? 1.0 : 0.0;
            }
            return 1.0;
        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            throw new NotImplementedException();
        }
    }
}
