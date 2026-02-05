using Avalonia.Data.Converters;
using GDMENUCardManager.Core;
using System;
using System.Globalization;

namespace GDMENUCardManager.Converter
{
    public class IsOpenMenuModeConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            if (value is MenuKind menuKind)
            {
                return menuKind == MenuKind.openMenu;
            }
            return false;
        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            throw new NotImplementedException();
        }
    }
}
