using System;
using System.Globalization;
using System.Windows.Data;
using GDMENUCardManager.Core;

namespace GDMENUCardManager.Converter
{
    public class IsNotMenuItemConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            if (value is GdItem item)
            {
                // Return false (disable) if this is a menu item, true (enable) if it's a game
                return !(item.Ip?.Name == "GDMENU" || item.Ip?.Name == "openMenu");
            }
            return true;
        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            throw new NotImplementedException();
        }
    }
}
