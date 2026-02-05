using System;
using System.IO;
using SixLabors.ImageSharp;
using SixLabors.ImageSharp.PixelFormats;
using SixLabors.ImageSharp.Processing;

namespace GDMENUCardManager.Core
{
    public static class PvrEncoder
    {
        // BOX.DAT constants (256x256)
        public const int TextureWidth = 256;
        public const int TextureHeight = 256;
        public const int PixelDataSize = TextureWidth * TextureHeight * 2;   // 131,072 bytes
        public const int TotalPvrSize = TotalHeaderSize + PixelDataSize;     // 131,104 bytes

        // ICON.DAT constants (128x128)
        public const int IconWidth = 128;
        public const int IconHeight = 128;
        public const int IconPixelDataSize = IconWidth * IconHeight * 2;     // 32,768 bytes
        public const int TotalIconPvrSize = TotalHeaderSize + IconPixelDataSize;  // 32,800 bytes

        // Common constants
        public const uint GlobalIndex = 1001;
        public const int GbixHeaderSize = 16;  // GBIX(4) + size(4) + index(4) + pad(4)
        public const int PvrtHeaderSize = 16;  // PVRT(4) + size(4) + format(2) + pad(2) + w(2) + h(2)
        public const int TotalHeaderSize = GbixHeaderSize + PvrtHeaderSize;  // 32 bytes

        // PVRT format constants
        private const byte PixelFormatRgb565 = 0x01;
        private const byte DataFormatSquareTwiddled = 0x01;

        /// <summary>
        /// Encode an image file to 256x256 PVR format for BOX.DAT (RGB565 TWIDDLED, Global Index 1001).
        /// Supports PNG, JPEG, GIF, WebP, BMP, TIFF, TGA.
        /// </summary>
        public static byte[] EncodeFromFile(string imagePath)
        {
            using var image = Image.Load<Bgra32>(imagePath);
            return EncodeFromImage(image, TextureWidth, TextureHeight);
        }

        /// <summary>
        /// Encode an image file to 128x128 PVR format for ICON.DAT (RGB565 TWIDDLED, Global Index 1001).
        /// Supports PNG, JPEG, GIF, WebP, BMP, TIFF, TGA.
        /// </summary>
        public static byte[] EncodeIconFromFile(string imagePath)
        {
            using var image = Image.Load<Bgra32>(imagePath);
            return EncodeFromImage(image, IconWidth, IconHeight);
        }

        /// <summary>
        /// Encode a stream to 256x256 PVR format for BOX.DAT (RGB565 TWIDDLED, Global Index 1001).
        /// </summary>
        public static byte[] EncodeFromStream(Stream stream)
        {
            using var image = Image.Load<Bgra32>(stream);
            return EncodeFromImage(image, TextureWidth, TextureHeight);
        }

        /// <summary>
        /// Encode a stream to 128x128 PVR format for ICON.DAT (RGB565 TWIDDLED, Global Index 1001).
        /// </summary>
        public static byte[] EncodeIconFromStream(Stream stream)
        {
            using var image = Image.Load<Bgra32>(stream);
            return EncodeFromImage(image, IconWidth, IconHeight);
        }

        /// <summary>
        /// Encode an ImageSharp image to PVR format at specified dimensions.
        /// </summary>
        private static byte[] EncodeFromImage(Image<Bgra32> image, int targetWidth, int targetHeight)
        {
            // Resize to target dimensions (force, ignoring aspect ratio)
            image.Mutate(ctx => ctx.Resize(targetWidth, targetHeight));

            // Extract pixel data in linear order
            var pixelData = new ushort[targetWidth * targetHeight];
            image.ProcessPixelRows(accessor =>
            {
                for (int y = 0; y < targetHeight; y++)
                {
                    var rowSpan = accessor.GetRowSpan(y);
                    for (int x = 0; x < targetWidth; x++)
                    {
                        var pixel = rowSpan[x];
                        pixelData[y * targetWidth + x] = Bgra32ToRgb565(pixel);
                    }
                }
            });

            // Twiddle the pixel data
            var twiddledData = TwiddleTexture(pixelData, targetWidth, targetHeight);

            // Build the PVR file
            return BuildPvrFile(twiddledData, targetWidth, targetHeight);
        }

        /// <summary>
        /// Convert BGRA32 to RGB565.
        /// </summary>
        private static ushort Bgra32ToRgb565(Bgra32 pixel)
        {
            // RGB565: RRRR_RGGG_GGGB_BBBB
            int r = (pixel.R >> 3) & 0x1F;  // 5 bits
            int g = (pixel.G >> 2) & 0x3F;  // 6 bits
            int b = (pixel.B >> 3) & 0x1F;  // 5 bits
            return (ushort)((r << 11) | (g << 5) | b);
        }

        /// <summary>
        /// Twiddle (Morton code) pixel data for square textures.
        /// </summary>
        private static ushort[] TwiddleTexture(ushort[] linearData, int width, int height)
        {
            var twiddled = new ushort[width * height];

            for (int y = 0; y < height; y++)
            {
                for (int x = 0; x < width; x++)
                {
                    int mortonIndex = GetMortonIndex(x, y);
                    twiddled[mortonIndex] = linearData[y * width + x];
                }
            }

            return twiddled;
        }

        /// <summary>
        /// Calculate Morton code (Z-order curve) for given x, y coordinates.
        /// </summary>
        private static int GetMortonIndex(int x, int y)
        {
            // Interleave bits: x in odd positions, y in even positions (Dreamcast PVR standard)
            return (int)(Part1By1(x) << 1 | Part1By1(y));
        }

        /// <summary>
        /// Insert a 0 bit between each of the 16 low bits of x.
        /// Example: ABCDEFGH -> 0A0B0C0D0E0F0G0H
        /// </summary>
        private static uint Part1By1(int n)
        {
            uint x = (uint)n;
            x = (x | (x << 8)) & 0x00FF00FF;
            x = (x | (x << 4)) & 0x0F0F0F0F;
            x = (x | (x << 2)) & 0x33333333;
            x = (x | (x << 1)) & 0x55555555;
            return x;
        }

        /// <summary>
        /// Build the complete PVR file with headers.
        /// </summary>
        private static byte[] BuildPvrFile(ushort[] twiddledData, int width, int height)
        {
            int pixelDataSize = width * height * 2;
            int totalSize = TotalHeaderSize + pixelDataSize;
            var pvr = new byte[totalSize];

            using var ms = new MemoryStream(pvr);
            using var writer = new BinaryWriter(ms);

            // GBIX header (16 bytes)
            writer.Write((byte)'G');
            writer.Write((byte)'B');
            writer.Write((byte)'I');
            writer.Write((byte)'X');
            writer.Write((uint)8);            // Size of data following (index + padding = 8)
            writer.Write((uint)GlobalIndex);  // Global Index value
            writer.Write((uint)0);            // Padding

            // PVRT header (16 bytes)
            writer.Write((byte)'P');
            writer.Write((byte)'V');
            writer.Write((byte)'R');
            writer.Write((byte)'T');
            writer.Write((uint)(pixelDataSize + 8));  // Data size (pixel data + 8 for format info)
            writer.Write(PixelFormatRgb565);          // Pixel format (1 byte)
            writer.Write(DataFormatSquareTwiddled);   // Data format (1 byte)
            writer.Write((ushort)0);                   // Padding
            writer.Write((ushort)width);               // Width
            writer.Write((ushort)height);              // Height

            // Pixel data
            foreach (var pixel in twiddledData)
            {
                writer.Write(pixel);
            }

            return pvr;
        }

        /// <summary>
        /// Decode PVR data to BGRA32 pixel array.
        /// Supports both 256x256 (BOX.DAT) and 128x128 (ICON.DAT) PVRs.
        /// Returns (pixelData, width, height) or null if invalid.
        /// </summary>
        public static (byte[] pixels, int width, int height)? DecodePvr(byte[] pvrData)
        {
            if (pvrData == null || pvrData.Length < TotalHeaderSize)
                return null;

            using var ms = new MemoryStream(pvrData);
            using var reader = new BinaryReader(ms);

            // Verify GBIX header
            if (reader.ReadByte() != 'G' ||
                reader.ReadByte() != 'B' ||
                reader.ReadByte() != 'I' ||
                reader.ReadByte() != 'X')
                return null;

            reader.ReadUInt32();  // GBIX size (skip)
            reader.ReadUInt32();  // Global index (skip)
            reader.ReadUInt32();  // Padding (skip)

            // Verify PVRT header
            if (reader.ReadByte() != 'P' ||
                reader.ReadByte() != 'V' ||
                reader.ReadByte() != 'R' ||
                reader.ReadByte() != 'T')
                return null;

            uint dataSize = reader.ReadUInt32();
            byte pixelFormat = reader.ReadByte();
            byte dataFormat = reader.ReadByte();
            reader.ReadUInt16();  // Padding
            ushort width = reader.ReadUInt16();
            ushort height = reader.ReadUInt16();

            if (pixelFormat != PixelFormatRgb565 || dataFormat != DataFormatSquareTwiddled)
                return null;

            // Validate supported dimensions (256x256 for BOX or 128x128 for ICON)
            bool isValidBox = (width == TextureWidth && height == TextureHeight);
            bool isValidIcon = (width == IconWidth && height == IconHeight);
            if (!isValidBox && !isValidIcon)
                return null;

            // Validate data length
            int expectedPixelDataSize = width * height * 2;
            int expectedTotalSize = TotalHeaderSize + expectedPixelDataSize;
            if (pvrData.Length < expectedTotalSize)
                return null;

            // Read twiddled pixel data
            var twiddledData = new ushort[width * height];
            for (int i = 0; i < twiddledData.Length; i++)
            {
                twiddledData[i] = reader.ReadUInt16();
            }

            // Untwiddle
            var linearData = UntwiddleTexture(twiddledData, width, height);

            // Convert to BGRA32
            var pixels = new byte[width * height * 4];
            for (int i = 0; i < linearData.Length; i++)
            {
                var rgb565 = linearData[i];
                int r = ((rgb565 >> 11) & 0x1F) << 3;
                int g = ((rgb565 >> 5) & 0x3F) << 2;
                int b = (rgb565 & 0x1F) << 3;

                // Expand lower bits for better color reproduction
                r |= (r >> 5);
                g |= (g >> 6);
                b |= (b >> 5);

                int offset = i * 4;
                pixels[offset + 0] = (byte)b;      // B
                pixels[offset + 1] = (byte)g;      // G
                pixels[offset + 2] = (byte)r;      // R
                pixels[offset + 3] = 255;          // A
            }

            return (pixels, width, height);
        }

        /// <summary>
        /// Untwiddle pixel data from Morton code to linear.
        /// </summary>
        private static ushort[] UntwiddleTexture(ushort[] twiddledData, int width, int height)
        {
            var linear = new ushort[width * height];

            for (int y = 0; y < height; y++)
            {
                for (int x = 0; x < width; x++)
                {
                    int mortonIndex = GetMortonIndex(x, y);
                    linear[y * width + x] = twiddledData[mortonIndex];
                }
            }

            return linear;
        }

        /// <summary>
        /// Decode PVR data and save as PNG to the specified path.
        /// Returns true if successful, false otherwise.
        /// </summary>
        public static bool SavePvrAsPng(byte[] pvrData, string outputPath)
        {
            var decoded = DecodePvr(pvrData);
            if (decoded == null)
                return false;

            var (pixels, width, height) = decoded.Value;

            using var image = Image.LoadPixelData<Bgra32>(pixels, width, height);
            image.SaveAsPng(outputPath);
            return true;
        }

        /// <summary>
        /// Decode PVR data and return as PNG byte array.
        /// Returns null if decoding fails.
        /// </summary>
        public static byte[] ConvertPvrToPngBytes(byte[] pvrData)
        {
            var decoded = DecodePvr(pvrData);
            if (decoded == null)
                return null;

            var (pixels, width, height) = decoded.Value;

            using var image = Image.LoadPixelData<Bgra32>(pixels, width, height);
            using var ms = new MemoryStream();
            image.SaveAsPng(ms);
            return ms.ToArray();
        }

        /// <summary>
        /// Downscale a 256x256 BOX.DAT PVR entry to 128x128 ICON.DAT format.
        /// Returns null if the input is not a valid 256x256 PVR.
        /// </summary>
        public static byte[] DownscaleBoxPvrToIcon(byte[] boxPvrData)
        {
            var decoded = DecodePvr(boxPvrData);
            if (decoded == null)
                return null;

            var (pixels, width, height) = decoded.Value;

            // Only support downscaling from 256x256
            if (width != TextureWidth || height != TextureHeight)
                return null;

            // Create ImageSharp image from decoded pixels
            using var image = Image.LoadPixelData<Bgra32>(pixels, width, height);

            // Encode as 128x128 icon
            return EncodeFromImage(image, IconWidth, IconHeight);
        }
    }
}
