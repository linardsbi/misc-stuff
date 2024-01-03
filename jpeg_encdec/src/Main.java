// AUTORS: <studenta apl. nr., vārds, uzvārds, grupa>

import javax.imageio.ImageIO;
import java.awt.*;
import java.awt.image.BufferedImage;
import java.awt.image.DataBufferInt;
import java.io.*;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.*;
import java.util.List;

final class YCbCrColor {
    public int Y;
    public int Cb;
    public int Cr;

    public YCbCrColor(int Y, int Cb, int Cr) {
        this.Y = Y;
        this.Cb = Cb;
        this.Cr = Cr;
    }

    public YCbCrColor(int RGB_pixel) {
        // Y komponents glabā pikseļa spilgtuma informāciju, kamēr Cb un Cr glabā atšķirību starp zilo krāsu un attēlu, un sarkano krāsu un attēlu
        // konstantes no: https://en.wikipedia.org/wiki/YCbCr#JPEG_conversion
        var pixel = new Color(RGB_pixel, true);
        Y = (int) ((0.299 * pixel.getRed()) + (0.587 * pixel.getGreen()) + (0.114 * pixel.getBlue()));
        Cb = (int) (128 - (0.168736 * pixel.getRed()) - (0.331264 * pixel.getGreen()) + (0.5 * pixel.getBlue()));
        Cr = (int) (128 + (0.5 * pixel.getRed()) - (0.418688 * pixel.getGreen()) - (0.081312 * pixel.getBlue()));

        // lai saspiestu attēlu vēl vairāk, krāsu atšķirību komponentus varam 'noapaļot' (jo vizuāli atšķirība praktiski nebūs redzama)
        Cb = (int) (2 * Math.round(Cb / 2.0));
        Cr = (int) (2 * Math.round(Cr / 2.0));
    }

    public YCbCrColor(YCbCrColor color) {
        Y = color.Y;
        Cb = color.Cb;
        Cr = color.Cr;
    }

    public int getComponent(byte index) {
        switch (index) {
            case 0:
                return Y;
            case 1:
                return Cb;
            case 2:
                return Cr;
        }

        return -1;
    }

    public void setComponent(byte index, int value) {
        switch (index) {
            case 0:
                Y = value;
                break;
            case 1:
                Cb = value;
                break;
            case 2:
                Cr = value;
                break;
        }
    }
};

final class Huffman {

    static class EncodedPair implements Serializable {
        String text;
        HashMap<Integer, String> codes;

        public EncodedPair(String string, HashMap<Integer, String> map) {
            text = string;
            codes = map;
        }
    }

    static class Node {
        Node(int sym, int freq) {
            frequency = freq;
            symbol = sym;
        }

        int frequency;
        int symbol;

        Node left = null;
        Node right = null;
    }

    private final HashMap<Integer, String> codes = new HashMap<>();

    private void tree_to_codes(Node root, String code) {
        if (root.right == null && root.left == null && root.symbol != Integer.MAX_VALUE) {
            codes.put(root.symbol, code);
            return;
        }
        if (root.left != null) {
            tree_to_codes(root.left, code + "0");
        }
        if (root.right != null) {
            tree_to_codes(root.right, code + "1");
        }
    }

    public void build_tree(HashMap<Integer, Integer> histogram) {
        PriorityQueue<Node> min_tree = new PriorityQueue<>(histogram.size(), (o1, o2) -> o1.frequency - o2.frequency);
        // aizpildam min_tree ar histogrammas vērtībām, kas automātiski sakārtosies augošā secībā
        histogram.forEach((value, occurrences) -> min_tree.add(new Node(value, occurrences)));

        // Savienojam visas histogrammas vērtības vienā kokā, sākot no visretāk sastopamās vērtības līdz visbiežāk sastopamajai.
        while (min_tree.size() > 1) {
            var left = min_tree.poll();
            var right = min_tree.poll();
            var parent = new Node(Integer.MAX_VALUE, left.frequency + right.frequency);

            parent.left = left;
            parent.right = right;

            min_tree.add(parent);
        }

        // Kad visbiežāk lietotajam mezglam ir pakārtoti visi pārējie mezgli, varam noteikt vērtībām atbilstošos kodus
        var root = min_tree.poll();

        if (root != null) {
            tree_to_codes(root, "");
        }
    }

    public List<Integer> decode(EncodedPair encoded) {
        List<Integer> values = new ArrayList<>();
        StringBuilder code = new StringBuilder();

        HashMap<String, Integer> huff_to_values = new HashMap<>();
        // reverse hashmap
        for (var key : encoded.codes.keySet()) {
            huff_to_values.put(encoded.codes.get(key), key);
        }

        for (int i = 0; i < encoded.text.length(); i++) {
            code.append(encoded.text.charAt(i));
            var current = code.toString();
            if (huff_to_values.containsKey(current)) {
                values.add(huff_to_values.get(current));
                code.setLength(0);
            }
        }

        return values;
    }

    public EncodedPair encode(List<Integer> values) {
        // avots: https://brilliant.org/wiki/huffman-encoding/
        // Hafmena kodēšanas pamatprincips ir sakārtot katru kodējamo simbolu kokā, pēc to parādīšanās biežuma, lai
        // visbiežāk izmantotie simboli sastāvētu no mazāk bitiem, nekā reti lietoti simboli.
        // Šī tipa kodēšana ir izdevīgāka par tādu, kas katru simbolu iekodē ar fiksēta garuma datiem,
        // tikai ja nepieciešams iekodēt plašu simbolu klāstu, kurā konkrēti simboli daudz atkārtojas.

        StringBuilder output = new StringBuilder();
        for (var value : values) {
            assert codes.containsKey(value);
            output.append(codes.get(value));
        }

        return new EncodedPair(output.toString(), codes);
    }
}

record CompressedJPEG(Huffman.EncodedPair encoded, int width, int height) implements Serializable {
}

final class JPEG {

    private int[] rgb_image;
    private final int width;
    private final int height;
    private final HashMap<Integer, Integer> histogram = new HashMap<>();

    public JPEG(CompressedJPEG encoded) {
        width = encoded.width();
        height = encoded.height();
        rgb_image = null;
    }

    public JPEG(BufferedImage image) {
        rgb_image = image.getRGB(0, 0, image.getWidth(), image.getHeight(), null, 0, image.getWidth());
        width = image.getWidth();
        height = image.getHeight();
    }

    // tabula nosaka JPEG attēla kvalitāti jeb to, cik daudz frekvenču vērtības tiks atmestas
    private static final int[] quantization_table = new int[]{16, 11, 10, 16, 24, 40, 51, 61, 12, 12, 14, 19, 26, 28, 60, 55, 14, 13, 16, 24, 40, 57, 69, 56, 14, 17, 22, 29, 51, 87, 80, 62, 18, 22, 37, 56, 68, 109, 103, 77, 24, 35, 55, 64, 81, 104, 113, 92, 49, 64, 78, 87, 103, 121, 120, 101, 72, 92, 95, 98, 112, 100, 103, 99};

    private void update_histogram(int value) {
        var occurrences = histogram.getOrDefault(value, 0);
        histogram.put(value, occurrences + 1);
    }

    private static YCbCrColor[] rgb_to_ycbcr(int[] rgb_image, int width, int height) {
        YCbCrColor[] ycbcr_image = new YCbCrColor[width * height];

        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                ycbcr_image[y * width + x] = new YCbCrColor(rgb_image[y * width + x]);
            }
        }

        return ycbcr_image;
    }

    // kā strādā DCT:
    // https://www.youtube.com/watch?v=DS8N8cFVd-E
    // https://www.youtube.com/watch?v=Q2aEzeMDHMA
    // https://www.youtube.com/watch?v=Ba89cI9eIg8
    private void apply_dct(YCbCrColor[] ycbcr_image, int x_offset, int y_offset) {
        YCbCrColor[] copied_block = new YCbCrColor[8 * 8];

        // izveidojam noteikto pikseļu kopiju, lai tos varētu izmantot transformācijas aprēķināšanā
        for (int y = 0; y < 8; y++) {
            for (int x = 0; x < 8; x++) {
                // Ja attēla platums vai augstums nedalās ar 8, kādā brīdī būs nepieciešams apstrādāt bloku, kas nav
                // 8x8. Tādā gadījumā izmantojam vērtības, kas atrodas attēla pašā malā/apakšā.
                var source_x = x_offset + x;
                var source_y = y_offset + y;

                if (source_x >= width) {
                    source_x = width - 1;
                }
                if (source_y >= height) {
                    source_y = height - 1;
                }

                var index = source_y * width + source_x;
                copied_block[y * 8 + x] = new YCbCrColor(ycbcr_image[index]);
            }
        }

        // algoritms: https://www.geeksforgeeks.org/discrete-cosine-transform-algorithm-program/
        for (int y1 = 0; y1 < 8; y1++) {
            for (int x1 = 0; x1 < 8; x1++) {
                // izlaižam transformāciju pikseļiem, kas atrodas ārpus attēla
                if ((y_offset + y1) >= height || (x_offset + x1) >= width) {
                    continue;
                }

                // kosinusa konstanšu formula atšķiras kreisās un augšējās malas pikseļiem
                var cx = x1 == 0 ? 1.0 / Math.sqrt(2) : 1.0;
                var cy = y1 == 0 ? 1.0 / Math.sqrt(2) : 1.0;

                // transformāciju jāaprēķina katram pikseļa komponentam
                for (byte i = 0; i < 3; i++) {
                    // summas mainīgajā tiks glabāta konkrētā pikseļa komponenta vērtības atbilstība katrai kosinusa
                    // funkciju bloka vērtībai.
                    double sum = 0.0;
                    for (int y = 0; y < 8; y++) {
                        for (int x = 0; x < 8; x++) {
                            sum += 0.25 * copied_block[y * 8 + x].getComponent(i) *
                                    Math.cos((2.0 * y + 1.0) * y1 * Math.PI / 16.0) *
                                    Math.cos((2.0 * x + 1.0) * x1 * Math.PI / 16.0);
                        }
                    }

                    // Vecās pikseļa vērtības vietā saglabājam to, cik daudz pikseļa vērtība atbilst kosinusa funkcijai,
                    // kas atrodas tajā pašā indeksā.
                    // Kad katrs pikselis būs transformēts, bloks saturēs aprakstu par to,
                    // kādam pikseļu 'rakstam' šis bloks atbilst vislabāk.
                    ycbcr_image[(y_offset + y1) * width + x1 + x_offset].setComponent(i, (int) (cy * cx * sum));
                }
            }
        }
    }

    private void apply_idct(YCbCrColor[] ycbcr_image, int x_offset, int y_offset) {
        // dct un idct definīcijas: https://unix4lyfe.org/dct/
        YCbCrColor[] copied_block = new YCbCrColor[8 * 8];

        // izveidojam noteikto pikseļu kopiju, lai tos varētu izmantot transformācijas aprēķināšanā
        for (int y = 0; y < 8; y++) {
            for (int x = 0; x < 8; x++) {
                // Ja attēla platums vai augstums nedalās ar 8, kādā brīdī būs nepieciešams apstrādāt bloku, kas nav
                // 8x8. Tādā gadījumā izmantojam vērtības, kas atrodas attēla pašā malā/apakšā.
                var source_x = x_offset + x;
                var source_y = y_offset + y;

                if (source_x >= width) {
                    source_x = width - 1;
                }
                if (source_y >= height) {
                    source_y = height - 1;
                }

                var index = source_y * width + source_x;
                copied_block[y * 8 + x] = new YCbCrColor(ycbcr_image[index]);
            }
        }

        for (int y = 0; y < 8; y++) {
            for (int x = 0; x < 8; x++) {
                // izlaižam transformāciju pikseļiem, kas atrodas ārpus attēla
                if ((y_offset + y) >= height || (x_offset + x) >= width) {
                    continue;
                }

                // transformāciju jāaprēķina katram pikseļa komponentam
                for (byte i = 0; i < 3; i++) {
                    double sum = 0.0;
                    for (int y1 = 0; y1 < 8; y1++) {
                        for (int x1 = 0; x1 < 8; x1++) {
                            // kosinusa konstanšu formula atšķiras kreisās un augšējās malas pikseļiem
                            var cx = x1 == 0 ? 1.0 / Math.sqrt(2) : 1.0;
                            var cy = y1 == 0 ? 1.0 / Math.sqrt(2) : 1.0;

                            sum += cx * cy * copied_block[y1 * 8 + x1].getComponent(i) *
                                    Math.cos((2.0 * y + 1.0) * y1 * Math.PI / 16.0) *
                                    Math.cos((2.0 * x + 1.0) * x1 * Math.PI / 16.0);
                        }
                    }

                    sum /= 4;

                    if (sum > 255.0) {
                        sum = 255;
                    } else if (sum < 0.0) {
                        sum = 0;
                    }

                    ycbcr_image[(y_offset + y) * width + x + x_offset].setComponent(i, (int) sum);
                }
            }
        }
    }

    private void apply_quantization(YCbCrColor[] ycbcr_image, int x_offset, int y_offset, boolean dequantization) {
        // Lai efektīvāk saspiestu attēlu, nepieciešams izfiltrēt augstas frekvences datus
        // (t.i., krāsu atšķirības starp pikseļiem, kuras cilvēka acs neredz) no attēla.
        // Tas tiek darīts izmantojot kvantizācijas procesu, kas frekvenci dala ar tai atbilstošu konstanti,
        // lai saglabātu zemas frekvences informāciju, kamēr augstas frekvences informācija tiek noapaļota uz 0
        for (int y = 0; y < 8; y++) {
            for (int x = 0; x < 8; x++) {
                // izlaižam kvantizāciju pikseļiem, kas atrodas ārpus attēla
                if ((y_offset + y) >= height || (x_offset + x) >= width) {
                    continue;
                }

                var table_i = y * 8 + x;
                var image_i = (y + y_offset) * width + x + x_offset;
                for (byte component = 0; component < 3; component++) {
                    var frequency = (float) ycbcr_image[image_i].getComponent(component);
                    int quantized_frequency;

                    if (dequantization) {
                        quantized_frequency = Math.round(frequency * quantization_table[table_i]);
                    } else {
                        quantized_frequency = Math.round(frequency / quantization_table[table_i]);
                    }

                    ycbcr_image[image_i].setComponent(component, quantized_frequency);
                }
            }
        }
    }

    interface ZigzagSetter {
        void call(int x, int y);
    }

    private void zigzag_add_list_value(YCbCrColor[] ycbcr_image, List<Integer> list, int x, int y) {
        var frequencies = ycbcr_image[y * width + x];
        list.add(frequencies.Y);
        list.add(frequencies.Cr);
        list.add(frequencies.Cb);
    }

    int offset = 0;

    private void zigzag_set_pixel_value(YCbCrColor[] ycbcr_image, List<Integer> list, int x, int y) {
        int Y = list.get(offset++);
        int Cr = list.get(offset++);
        int Cb = list.get(offset++);

        ycbcr_image[y * width + x] = new YCbCrColor(Y, Cb, Cr);
    }

    private void zigzag_block_visitor(int x_offset, int y_offset, ZigzagSetter setter_func) {
        // Izveidojam funkciju, kas neizsauks padoto funkciju, ja algoritms mēģinās iziet ārpus attēla robežām.
        // Šāda situācija ir iespējama, ja attēlu nevar sadalīt 8x8 pikseļu matricās.
        // Algoritmu varētu padarīt ātrāku, ja ciklā netiktu apstaigāti neesošie pikseļi.
        ZigzagSetter bounded_set = (int x_coord, int y_coord) -> {
            if (x_coord >= width || y_coord >= height) {
                return;
            }
            setter_func.call(x_coord, y_coord);
        };

        // Pats pirmais pikselis, algoritma implementācijas kontekstā, ir izņēmuma gadījums, tādēļ to pievienojam ārpus cikla
        setter_func.call(x_offset, y_offset);

        boolean zig = true;
        boolean second_half = false;

        for (int x = 0, y = 0; x <= 7 && y <= 7; bounded_set.call(x_offset + x, y_offset + y)) {
            if (x == 0 && y == 7) {
                // kad algoritms ir izgājis cauri pusei no bloka pikseļiem, tam jāmaina to, kā tas rīkojas, ja sasniedz
                // bloka malu.
                second_half = true;
            }

            boolean hit_edge = false;

            if (!second_half) {
                // ejot cauri pirmajai bloka pusei zigzagā, atsitoties pret kreiso malu zigzaga līnija paiet uz leju, nevis šķērsi,
                // turpretim, atsitoties pret augšējo malu, tā paiet pa labi.
                if (zig && y - 1 < 0) {
                    x++;
                    hit_edge = true;
                } else if (!zig && x - 1 < 0) {
                    y++;
                    hit_edge = true;
                }
            } else {
                // Otrajā bloka pusē, zigzaga līnija, atsitoties pret apakšu, paiet pa labi. Turpretim, atsitoties pret labo malu,
                // paiet uz leju.
                if (!zig && y + 1 > 7) {
                    x++;
                    hit_edge = true;
                } else if (zig && x + 1 > 7) {
                    y++;
                    hit_edge = true;
                }
            }

            if (hit_edge) {
                // ja atsitamies pret malu, tad līnija tālāk ies pretējā virzienā
                zig = !zig;
                continue;
            }

            // līnijas kustība pa labi un uz augšu (zig) vai pa kreisi un uz leju (zag)
            if (zig) {
                x++;
                y--;
            } else { // zag
                x--;
                y++;
            }

        }
    }

    private List<Integer> run_length_encode(List<Integer> zigzag_output) {
        List<Integer> output = new ArrayList<>();

        for (int i = 1, total = 1, value = zigzag_output.get(0); i <= zigzag_output.size(); i++, total++) {
            // iterējam ārpus 'zigzag_output' robežām, lai saraksta pēdējā vērtība arī tiktu saglabāta
            var zigzag_value = i < zigzag_output.size() ? zigzag_output.get(i) : Integer.MAX_VALUE;
            if (value != zigzag_value) {
                // fixme: might compress better if these are encoded as a pair and not separately
//                if (value == 0) {
//                    total = 0;
//                    value = zigzag_value;
//                    continue;
//                }
                output.add(value);
                output.add(total);
                update_histogram(value);
                update_histogram(total);
                total = 0;
                value = zigzag_value;
            }
        }

        return output;
    }

    private List<Integer> run_length_decode(List<Integer> input) {
        List<Integer> decoded_values = new ArrayList<>(input.size());
        for (int i = 0; i < input.size(); i += 2) {
            int value = input.get(i);
            int count = input.get(i + 1);
            decoded_values.addAll(Collections.nCopies(count, value));
        }

        return decoded_values;
    }

    public CompressedJPEG compress() {
        var ycbcr_image = rgb_to_ycbcr(rgb_image, width, height);
        List<Integer> run_length_encoded = new ArrayList<>();

        for (int y = 0; y < height; y += 8) {
            for (int x = 0; x < width; x += 8) {
                apply_dct(ycbcr_image, x, y);
                apply_quantization(ycbcr_image, x, y, false);

                List<Integer> zigzag_output = new ArrayList<>();
                zigzag_block_visitor(x, y, (int block_x, int block_y) -> zigzag_add_list_value(ycbcr_image, zigzag_output, block_x, block_y));

                run_length_encoded.addAll(run_length_encode(zigzag_output));
            }
        }

        // Lai attēls tiktu saspiests vēl vairāk, dažādo frekvenču datus būtu nepieciešams glabāt atsevišķi citu no cita.

        var coder = new Huffman();
        coder.build_tree(histogram);

        return new CompressedJPEG(coder.encode(run_length_encoded), width, height);
    }

    public void decompress(Huffman.EncodedPair encoded) {
        var coder = new Huffman();
        var value_list = coder.decode(encoded);

        YCbCrColor[] ycbcr_image = new YCbCrColor[width * height];
        var decoded_values = run_length_decode(value_list);

        for (int y = 0; y < height; y += 8) {
            for (int x = 0; x < width; x += 8) {
                zigzag_block_visitor(x, y, (int block_x, int block_y) -> zigzag_set_pixel_value(ycbcr_image, decoded_values, block_x, block_y));
                apply_quantization(ycbcr_image, x, y, true);
                apply_idct(ycbcr_image, x, y);
            }
        }

        ycbcr_to_rgb(ycbcr_image);
    }

    public BufferedImage create_image() {
        BufferedImage image = new BufferedImage(width, height, BufferedImage.TYPE_INT_RGB);
        var buffer = (DataBufferInt) image.getRaster().getDataBuffer();
        System.arraycopy(rgb_image, 0, buffer.getData(), 0, width * height);
        return image;
    }

    private int clamp(int value) {
        return Math.max(0, Math.min(255, value));
    }

    private void ycbcr_to_rgb(YCbCrColor[] ycbcr_image) {
        rgb_image = new int[width * height];

        // konstantes no: https://en.wikipedia.org/wiki/YCbCr#JPEG_conversion
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                var pixel = ycbcr_image[y * width + x];
                pixel.Cb -= 128;
                pixel.Cr -= 128;
                var r = clamp((int) (pixel.Y + 1.402 * (pixel.Cr)));
                var g = clamp((int) (pixel.Y - 0.344136 * (pixel.Cb) - 0.714136 * (pixel.Cr)));
                var b = clamp((int) (pixel.Y + 1.772 * (pixel.Cb)));

                // formatējam atbilstoši TYPE_INT_RGB (0x00RRGGBB)
                rgb_image[y * width + x] = (r << 16) | (g << 8) | b;
            }
        }
    }

    public void test_transform() {
        var ycbcr_image = rgb_to_ycbcr(rgb_image, width, height);

        for (int y = 0; y < height; y += 8) {
            for (int x = 0; x < width; x += 8) {
                apply_dct(ycbcr_image, x, y);
                apply_quantization(ycbcr_image, x, y, false);
            }
        }

        for (int y = 0; y < height; y += 8) {
            for (int x = 0; x < width; x += 8) {
                apply_quantization(ycbcr_image, x, y, true);
                apply_idct(ycbcr_image, x, y);
            }
        }

        ycbcr_to_rgb(ycbcr_image);
    }

    public void test_color_convert() {
        var ycbcr_image = rgb_to_ycbcr(rgb_image, width, height);
        ycbcr_to_rgb(ycbcr_image);
    }
}

public class Main {
    private static boolean handle_comp(String input_filename, String archive_filename) {
        BufferedImage image;

        try {
            image = ImageIO.read(new File(input_filename));
        } catch (IOException e) {
            return false;
        }

        var encoder = new JPEG(image);
        var compressed = encoder.compress();

        // Ierakstām CompressedJPEG objektu failā
        try (ByteArrayOutputStream bos = new ByteArrayOutputStream(); ObjectOutputStream oos = new ObjectOutputStream(bos)) {
            oos.writeObject(compressed);
            Files.write(Paths.get(archive_filename), bos.toByteArray());
        } catch (IOException e) {
            return false;
        }

        return true;
    }

    private static boolean handle_decomp(String archive_filename, String output_filename) {
        try (FileInputStream fileIn = new FileInputStream(archive_filename); ObjectInputStream objectIn = new ObjectInputStream(fileIn)) {
            // ielasām CompressedJPEG objektu (satur saspiestā attēla augstumu, platumu, Hafmena kodus un šifra tekstu)
            var compressed_jpeg = (CompressedJPEG) objectIn.readObject();
            var jpeg = new JPEG(compressed_jpeg);

            jpeg.decompress(compressed_jpeg.encoded());

            var image = jpeg.create_image();
            var extension_index = output_filename.lastIndexOf('.');

            if (extension_index == -1) {
                return false;
            }

            var format_name = output_filename.substring(extension_index + 1);

            // attēla formāts ir atkarīgs no norādītā izvadfaila formāta
            ImageIO.write(image, format_name, new File(output_filename));
        } catch (IOException | ClassNotFoundException e) {
            return false;
        }

        return true;
    }

    private static boolean handle_size(String filename) {
        Path path = Paths.get(filename);

        try {
            long size = Files.size(path);
            System.out.println("size: " + size);
        } catch (IOException e) {
            return false;
        }

        return true;
    }

    private static boolean handle_equal(String first_path, String second_path) {
        try {
            var is_equal = Files.mismatch(Paths.get(first_path), Paths.get(second_path)) == -1;
            System.out.println(is_equal);

        } catch (IOException e) {
            return false;
        }

        return true;
    }

    public static void test_comp() throws IOException {
        // without run-length encoding, with 0:    690k -> 1.2mb
        // without run-length encoding, without 0: 690k -> 258kb
        // with run-length encoding, with 0:    690k -> 587kb
        // with run-length encoding, without 0: 690k -> 290kb
        var archive_filename = "/home/hvssz/Downloads/comp";
        if (handle_comp("/home/hvssz/Downloads/space.bmp", archive_filename)) {
            Path output = Path.of(archive_filename);
            var file = Files.exists(output);
            System.out.println(file ? "archive created (" + Files.size(output) + "B)" : "failed to create archive");
        } else {
            System.out.println("invalid arguments");
        }
    }

    public static void test_decomp() throws IOException {
        var archive_filename = "/home/hvssz/Downloads/comp";
        var output_filename = "/home/hvssz/Downloads/decomp.bmp";
        if (handle_decomp(archive_filename, output_filename)) {
            Path output = Path.of(output_filename);
            var file = Files.exists(output);
            System.out.println(file ? "archive decompressed (" + Files.size(output) + "B)" : "failed to decompress archive");
        } else {
            System.out.println("invalid arguments");
        }
    }

    public static void test_JPEG() {
        var dir = "/home/hvssz/Downloads/";
        var input_filename = dir + "space.png";
        BufferedImage image;

        try {
            image = ImageIO.read(new File(input_filename));
        } catch (IOException e) {
            System.out.println("Could not read input file");
            return;
        }

        var encoder = new JPEG(image);
        encoder.test_color_convert();

        try {
            ImageIO.write(encoder.create_image(), "bmp", new File(dir + "convert.bmp"));
        } catch (IOException e) {
            System.out.println("Could not write color convert output file");
            return;
        }

        encoder.test_transform();

        try {
            ImageIO.write(encoder.create_image(), "bmp", new File(dir + "transform.bmp"));
        } catch (IOException e) {
            System.out.println("Could not write transform output file");
            return;
        }
    }

    public static void main(String[] args) throws IOException {
        boolean exit = false;
        Scanner sc = new Scanner(System.in);

        while (!exit) {
            System.out.println("enter a command:");
            String line = sc.nextLine();

            switch (line) {
                case "comp":
                    System.out.println("source file name:");
                    String filename = sc.nextLine();

                    System.out.println("archive name:");
                    String archive_name = sc.nextLine();

                    /* JPEG zudumradošais saspiešanas algoritms sastāv no šādiem soļiem:
                    *  1. Attēls tiek pārveidots uz YCbCr krāsu modeli, Cb un Cr vērtības tiek noapaļotas, jo atsķirības nebūs redzamas, bet tiks ietaupīta vieta atmiņā;
                    *  2. Attēls tiek pārveidots ar diskrēto kosinusa transformāciju, pārveidojot tā pikseļus frekvences domēnā
                    *  3. Frekvences tiek kvantizētas, noņemot konkrētu daudzumu (ko nosaka quantization_table konstantes) augstās frekvences informācijas (skat. komentārus apply_dct() funkcijā)
                    *  4. Attēla frekvenču vērtības tiek ievietotas masīvā, ejot katram 8x8 blokam cauri zigzaga veidā.
                    *     Šī soļa mērķis ir izveidot garu virkni ar vienādiem skaitļiem, kurus kompakti iespējams pārstāvēt ar 'run-length' kodētām vērtībām.
                    *  5. Frekvences tiek iekodētas izmantojot 'run-length' algoritmu
                    *  6. Rezultējošās vērtības tiek saglabātas histogrammā, no kuras tiek izveidoti Hafmena kodi, lai tiktu atrasta visīsākā iespējamā bitu sekvence, kas izsaka sākotnējo vērtību.
                    *  7. Tiek ģenerēts saspiestais teksts, kas tiek saglabāts failā, kopā ar attēla izmēru un Hafmena kodiem, lai vēlāk būtu iespējams attēlu atveidot pēc šiem parametriem.
                    */
                    if (!handle_comp(filename, archive_name)) {
                        System.out.println("invalid arguments");
                    }

                    break;
                case "decomp":
                    System.out.println("archive name:");
                    String archive_filename = sc.nextLine();

                    System.out.println("file name:");
                    String output_filename = sc.nextLine();

                    // Decomp dara gandrīz to pašu ko comp, tikai pretējā virzienā.
                    // Svarīgākais ir tas, ka no saspiestajiem datiem atgūtās frekvences tiek pārveidotas atpakaļ uz pikseļu vērtībām izmantojot inverso diskrēto kosinusa transformāciju.
                    if (!handle_decomp(archive_filename, output_filename)) {
                        System.out.println("invalid arguments");
                    }
                    break;
                case "about":
                    System.out.println("<studenta apl. nr., vārds, uzvārds, grupa>");
                    break;
                case "size":
                    System.out.println("file name:");
                    String input_filename = sc.nextLine();

                    if (!handle_size(input_filename)) {
                        System.out.println("invalid argument");
                    }
                    break;
                case "equal":
                    System.out.println("first file name:");
                    String first_filename = sc.nextLine();

                    System.out.println("second file name:");
                    String second_filename = sc.nextLine();

                    if (!handle_equal(first_filename, second_filename)) {
                        System.out.println("invalid arguments");
                    }
                    break;
                case "exit":
                    exit = true;
                    break;
                default:
                    System.out.println("wrong command");
            }
        }
    }
}