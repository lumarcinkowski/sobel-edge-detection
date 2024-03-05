#include <iostream>
#include <fstream>
#include <cmath>
using namespace std;

struct BITMAPFILEHEADER {
    unsigned char bfType[2];
    unsigned char bfSize[4];
    unsigned char bfReserved1[2];
    unsigned char bfReserved2[2];
    unsigned char bfOffBits[4];
};
struct BITMAPFILEHEADERINT {
    string bfType = {};
    int bfSize = 0;
    int bfReserved1 = 0;
    int bfReserved2 = 0;
    int bfOffBits = 0;
};
struct BITMAPINFOHEADER {
    unsigned char biSize[4];
    unsigned char biWidth[4];
    unsigned char biHeight[4];
    unsigned char biPlanes[2];
    unsigned char biBitCount[2];
    unsigned char biCompression[4];
    unsigned char biSizeImage[4];
    unsigned char biXpelsPerMeter[4];
    unsigned char biYpelsPerMeter[4];
    unsigned char biCrlUses[4];
    unsigned char biCrlImportant[4];
};
struct BITMAPINFOHEADERINT {
    int biSize = 0;
    int biWidth = 0;
    int biHeight = 0;
    int biPlanes = 0;
    int biBitCount = 0;
    int biCompression = 0;
    int biSizeImage = 0;
    int biXpelsPerMeter = 0;
    int biYpelsPerMeter = 0;
    int biCrlUses = 0;
    int biCrlImportant = 0;
};
int readBFH(ifstream& ifs, BITMAPFILEHEADER& bfh);
int readBIH(ifstream& ifs, BITMAPINFOHEADER& bih);
int change_values(int liczba, int potega);
void convertBFH(BITMAPFILEHEADER bfh, BITMAPFILEHEADERINT& bfh2);
void convertBIH(BITMAPINFOHEADER bih, BITMAPINFOHEADERINT& bih2);
void writeBFH(BITMAPFILEHEADERINT bfh2);
void writeBIH(BITMAPINFOHEADERINT& bih);
void download_colors(ifstream& plik, int** blue, int** green, int** red, BITMAPINFOHEADERINT bih);
void sobel(int** color, int rows, int cols, int** out_image);
void delete_tab(int** tab, BITMAPINFOHEADERINT bih2);


int main()
{
    ifstream file;
    ofstream out_file;
    string extension = ".bmp";
    string file_name;
    string out_file_name;
    string all_file_name;
    string all_out_file_name;

    cout << "Podaj nazwe pliku wejsciowego (bez rozszerzenia) " << endl;
    cin >> file_name;
    all_file_name = file_name + extension;
    file.open(all_file_name, ios::binary);
    if (file.good() == false) {
        cout << "Nie wczytano poprawnie pliku" << endl;
        exit(0);

    }



    cout << "Podaj nazwe pliku wyjsciowego (bez rozszerzenia) " << endl;
    cin >> out_file_name;
    all_out_file_name = out_file_name + extension;
    out_file.open(all_out_file_name, ios::binary);
    if (out_file.good() == false) {
        cout << "Nie wczytano pliku";
        exit(0);

    }




    system("cls");
    BITMAPFILEHEADER file_header;
    BITMAPFILEHEADERINT file_header_int;
    BITMAPINFOHEADER info_header;
    BITMAPINFOHEADERINT info_header_int;
    readBFH(file, file_header);
    readBIH(file, info_header);
    convertBFH(file_header, file_header_int);
    writeBFH(file_header_int);
    convertBIH(info_header, info_header_int);
    writeBIH(info_header_int);


    file.seekg(0, file.beg);
    char sign[1];
    while (file.tellg() < file_header_int.bfOffBits)
    {
        file.read(sign, 1);
        out_file.write(sign, 1);
    }

    int** blue = new int* [info_header_int.biHeight];
    for (int i = 0; i < info_header_int.biHeight; i++) {
        blue[i] = new int[info_header_int.biWidth];

    }
    int** green = new int* [info_header_int.biHeight];
    for (int i = 0; i < info_header_int.biHeight; i++) {

        green[i] = new int[info_header_int.biWidth];
    }
    int** red = new int* [info_header_int.biHeight];
    for (int i = 0; i < info_header_int.biHeight; i++) {
        red[i] = new int[info_header_int.biWidth];

    }

    download_colors(file, blue, green, red, info_header_int);

    int** out_image_B = new int* [info_header_int.biHeight];
    for (int i = 0; i < info_header_int.biHeight; i++) {
        out_image_B[i] = new int[info_header_int.biWidth];
    }
    int** out_image_G = new int* [info_header_int.biHeight];
    for (int i = 0; i < info_header_int.biHeight; i++) {
        out_image_G[i] = new int[info_header_int.biWidth];
    }
    int** out_image_R = new int* [info_header_int.biHeight];
    for (int i = 0; i < info_header_int.biHeight; i++) {
        out_image_R[i] = new int[info_header_int.biWidth];
    }
    int x = info_header_int.biHeight;
    int y = info_header_int.biWidth;
    sobel(blue, x, y, out_image_B);
    sobel(green, x, y, out_image_G);
    sobel(red, x, y, out_image_R);
    int zero = 0;
    for (int i = 0; i < info_header_int.biHeight; i++) {
        for (int j = 0; j < info_header_int.biWidth; j++)
        {
            out_file.write(reinterpret_cast<char*>(&out_image_B[i][j]), 1);
            out_file.write(reinterpret_cast<char*>(&out_image_G[i][j]), 1);
            out_file.write(reinterpret_cast<char*>(&out_image_R[i][j]), 1);
        }
        for (int k = 0; k < info_header_int.biWidth % 4; k++)
        {
            out_file.write((const char*)&zero, 1);
        }
    }
    delete_tab(blue, info_header_int);
    delete_tab(green, info_header_int);
    delete_tab(red, info_header_int);
    delete_tab(out_image_B, info_header_int);
    delete_tab(out_image_G, info_header_int);
    delete_tab(out_image_R, info_header_int);
    file.close();
    out_file.close();



    return 0;
}
int readBFH(ifstream& ifs, BITMAPFILEHEADER& bfh) {
    ifs.read(reinterpret_cast<char*>(&bfh.bfType), 2);
    ifs.read(reinterpret_cast<char*>(&bfh.bfSize), 4);
    ifs.read(reinterpret_cast<char*>(&bfh.bfReserved1), 2);
    ifs.read(reinterpret_cast<char*>(&bfh.bfReserved2), 2);
    ifs.read(reinterpret_cast<char*>(&bfh.bfOffBits), 4);
    return ifs.tellg();
}
int readBIH(ifstream& ifs, BITMAPINFOHEADER& bih) {
    ifs.read(reinterpret_cast<char*>(&bih.biSize), 4);
    ifs.read(reinterpret_cast<char*>(&bih.biWidth), 4);
    ifs.read(reinterpret_cast<char*>(&bih.biHeight), 4);
    ifs.read(reinterpret_cast<char*>(&bih.biPlanes), 2);
    ifs.read(reinterpret_cast<char*>(&bih.biBitCount), 2);
    ifs.read(reinterpret_cast<char*>(&bih.biCompression), 4);
    ifs.read(reinterpret_cast<char*>(&bih.biSizeImage), 4);
    ifs.read(reinterpret_cast<char*>(&bih.biXpelsPerMeter), 4);
    ifs.read(reinterpret_cast<char*>(&bih.biYpelsPerMeter), 4);
    ifs.read(reinterpret_cast<char*>(&bih.biCrlUses), 4);
    ifs.read(reinterpret_cast<char*>(&bih.biCrlImportant), 4);
    return ifs.tellg();
}
void convertBIH(BITMAPINFOHEADER bih, BITMAPINFOHEADERINT& bih2) {
    int number;
    int result = 0;
    for (int i = 0; i < 4; i++) {
        number = bih.biSize[i];
        result += change_values(number, i);
    }
    bih2.biSize = result;
    result = 0;
    for (int i = 0; i < 4; i++) {
        number = bih.biWidth[i];
        result += change_values(number, i);
    }
    bih2.biWidth = result;
    result = 0;
    for (int i = 0; i < 4; i++) {
        number = bih.biHeight[i];
        result += change_values(number, i);
    }
    bih2.biHeight = result;
    result = 0;

    for (int i = 0; i < 2; i++) {
        number = bih.biPlanes[i];
        result += change_values(number, i);
    }
    bih2.biPlanes = result;
    result = 0;

    for (int i = 0; i < 2; i++) {
        number = bih.biBitCount[i];
        result += change_values(number, i);
    }
    bih2.biBitCount = result;
    result = 0;

    for (int i = 0; i < 4; i++) {
        number = bih.biCompression[i];
        result += change_values(number, i);
    }
    bih2.biCompression = result;
    result = 0;
    for (int i = 0; i < 4; i++) {
        number = bih.biSizeImage[i];
        result += change_values(number, i);
    }
    bih2.biSizeImage = result;
    result = 0;

    for (int i = 0; i < 4; i++) {
        number = bih.biXpelsPerMeter[i];
        result += change_values(number, i);
    }
    bih2.biXpelsPerMeter = result;
    result = 0;

    for (int i = 0; i < 4; i++) {
        number = bih.biYpelsPerMeter[i];
        result += change_values(number, i);
    }
    bih2.biYpelsPerMeter = result;
    result = 0;
    for (int i = 0; i < 4; i++) {
        number = bih.biCrlUses[i];
        result += change_values(number, i);
    }
    bih2.biCrlUses = result;
    result = 0;

    for (int i = 0; i < 4; i++) {
        number = bih.biCrlImportant[i];
        result += change_values(number, i);
    }
    bih2.biCrlImportant = result;
    result = 0;

}
int change_values(int liczba, int potega) {
    return liczba * pow(256, potega);

}
void convertBFH(BITMAPFILEHEADER bfh, BITMAPFILEHEADERINT& bfh2) {
    int result = 0;
    int number;
    for (int i = 0; i < 2; i++) {
        bfh2.bfType += bfh.bfType[i];
    }

    for (int i = 0; i < 4; i++) {
        number = bfh.bfSize[i];
        result += change_values(number, i);
    }
    bfh2.bfSize = result;
    result = 0;


    for (int i = 0; i < 2; i++) {
        number = (bfh.bfReserved1[i]);
        result += change_values(number, i);
    }
    bfh2.bfReserved1 = result;
    result = 0;

    for (int i = 0; i < 2; i++) {
        number = (bfh.bfReserved2[i]);
        result += change_values(number, i);
    }
    bfh2.bfReserved2 = result;
    result = 0;
    for (int i = 0; i < 4; i++) {
        number = bfh.bfOffBits[i];
        result += change_values(number, i);
    }
    bfh2.bfOffBits = result;
}
void writeBIH(BITMAPINFOHEADERINT& bih) {
    cout << "BITMAPINFOHEADER " << endl;
    cout << "bi.Size: " << bih.biSize << endl;
    cout << "bi.Width: " << bih.biWidth << endl;
    cout << "bi.Height: " << bih.biHeight << endl;
    cout << "bi.Planes: " << bih.biPlanes << endl;
    cout << "bi.BitCount: " << bih.biBitCount << endl;
    cout << "bi.Compression: " << bih.biCompression << endl;
    cout << "bi.SizeImage: " << bih.biSizeImage << endl;
    cout << "bi.XpelsPerMeter: " << bih.biXpelsPerMeter << endl;
    cout << "bi.YpelsPerMeter: " << bih.biYpelsPerMeter << endl;
    cout << "bi.CrlUses: " << bih.biCrlUses << endl;
    cout << "bi.CrlImportant: " << bih.biCrlImportant << endl;


}


void writeBFH(BITMAPFILEHEADERINT bfh2) {
    cout << "BITMAPFILEHEADER" << endl;
    cout << "bfType: " << bfh2.bfType << endl;
    cout << "bfSize: " << bfh2.bfSize << endl;
    cout << "bfReserved1: " << bfh2.bfReserved1 << endl;
    cout << "bfReserved2:" << bfh2.bfReserved2 << endl;
    cout << "bfOffBits:" << bfh2.bfOffBits << endl;
    cout << endl;
}
void download_colors(ifstream& plik, int** blue, int** green, int** red, BITMAPINFOHEADERINT bih) {

    for (int i = 0; i < bih.biHeight; i++) {
        for (int j = 0; j < bih.biWidth; j++) {

            unsigned char pixel[3];
            plik.read(reinterpret_cast<char*>(pixel), 3);
            blue[i][j] = pixel[0];
            green[i][j] = pixel[1];
            red[i][j] = pixel[2];
        }
        for (int k = 0; k < bih.biWidth % 4; k++) {
            unsigned char pixel0[1];
            plik.read(reinterpret_cast<char*>(pixel0), 1);


        }
    }
    //return plik.tellg();

}
void sobel(int** color, int rows, int cols, int** out_image) {
    short sobel_mask_0[3][3] = {
    { 1, 2, 1},
    { 0, 0, 0},
    {-1, -2, -1} };
    short sobel_mask_1[3][3] = {
    { 2, 1, 0},
    { 1, 0, -1},
    { 0, -1, -2} };
    short sobel_mask_2[3][3] = {
    { 1, 0, -1},
    { 2, 0, -2},
    { 1, 0, -1} };
    short sobel_mask_3[3][3] = {
    { 0, -1, -2},
    { 1, 0, -1},
    { 2, 1, 0} };
    short sobel_mask_4[3][3] = {
    {-1, -2, -1},
    { 0, 0, 0},
    { 1, 2, 1} };
    short sobel_mask_5[3][3] = {
    {-2, -1, 0},
    {-1, 0, 1},
    { 0, 1, 2} };
    short sobel_mask_6[3][3] = {
    {-1, 0, 1},
    {-2, 0, 2},
    {-1, 0, 1} };
    short sobel_mask_7[3][3] = {
    { 0, 1, 2},
    {-1, 0, 1},
    {-2, -1, 0} };
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            out_image[i][j] = 0;
        }
    }

    int min = 0;
    int max = 255;
    int sum = 0;
    int a, b;


    for (int i = 1; i < rows - 1; i++) {

        for (int j = 1; j < cols - 1; j++) {

            //0
            for (int a = -1; a < 2; a++) {
                for (int b = -1; b < 2; b++) {
                    sum = sum + color[i + a][j + b] * sobel_mask_0[a + 1][b + 1];
                }
            }
            if (sum > max) sum = max;
            if (sum < 0) sum = 0;
            if (sum > out_image[i][j])      out_image[i][j] = sum;

            //1mask
            sum = 0;
            for (a = -1; a < 2; a++) {
                for (b = -1; b < 2; b++) {
                    sum = sum + color[i + a][j + b] * sobel_mask_1[a + 1][b + 1];
                }
            }
            if (sum > max) sum = max;
            if (sum < 0) sum = 0;
            if (sum > out_image[i][j]) out_image[i][j] = sum;


            //2
            sum = 0;
            for (a = -1; a < 2; a++) {
                for (b = -1; b < 2; b++) {
                    sum = sum + color[i + a][j + b] * sobel_mask_2[a + 1][b + 1];
                }
            }
            if (sum > max) sum = max;
            if (sum < 0) sum = 0;

            if (sum > out_image[i][j])      out_image[i][j] = sum;


            //3
            sum = 0;
            for (a = -1; a < 2; a++) {
                for (b = -1; b < 2; b++) {
                    sum = sum + color[i + a][j + b] * sobel_mask_3[a + 1][b + 1];
                }
            }
            if (sum > max) sum = max;
            if (sum < 0) sum = 0;

            if (sum > out_image[i][j])      out_image[i][j] = sum;


            //4
            sum = 0;
            for (a = -1; a < 2; a++) {
                for (b = -1; b < 2; b++) {
                    sum = sum + color[i + a][j + b] * sobel_mask_4[a + 1][b + 1];
                }
            }
            if (sum > max) sum = max;
            if (sum < 0) sum = 0;

            if (sum > out_image[i][j])      out_image[i][j] = sum;


            //5
            sum = 0;
            for (a = -1; a < 2; a++) {
                for (b = -1; b < 2; b++) {
                    sum = sum + color[i + a][j + b] * sobel_mask_5[a + 1][b + 1];
                }
            }
            if (sum > max) sum = max;
            if (sum < 0) sum = 0;

            if (sum > out_image[i][j])      out_image[i][j] = sum;

            //6
            sum = 0;
            for (a = -1; a < 2; a++) {
                for (b = -1; b < 2; b++) {
                    sum = sum + color[i + a][j + b] * sobel_mask_6[a + 1][b + 1];
                }
            }
            if (sum > max) sum = max;
            if (sum < 0) sum = 0;

            if (sum > out_image[i][j])      out_image[i][j] = sum;


            //7
            sum = 0;
            for (a = -1; a < 2; a++) {
                for (b = -1; b < 2; b++) {
                    sum = sum + color[i + a][j + b] * sobel_mask_7[a + 1][b + 1];
                }
            }
            if (sum > max) sum = max;
            if (sum < 0) sum = 0;

            if (sum > out_image[i][j])      out_image[i][j] = sum;



        }
    }
}
void delete_tab(int** tab, BITMAPINFOHEADERINT bih2) {
    for (int i = 0; i < bih2.biHeight; i++) {
        delete[] tab[i];

    }
    delete[] tab;
}