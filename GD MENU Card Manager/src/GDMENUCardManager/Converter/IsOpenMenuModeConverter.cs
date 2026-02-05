using System;
using System.Globalization;
using System.Windows;
using System.Windows.Data;
using GDMENUCardManager.Core;

namespace GDMENUCardManager.Converter
{
    public class IsOpenMenuModeConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            if (value is MenuKind menuKind)
            {
                return menuKind == MenuKind.openMenu ? Visibility.Visible : Visibility.Collapsed;
            }
            return Visibility.Collapsed;
        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            throw new NotImplementedException();
        }
    }
}
