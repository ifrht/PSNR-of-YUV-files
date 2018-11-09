#include <iostream>
#include <string>
#include <stdio.h>
#include <math.h>
#include <iomanip>

using namespace std;

/*  argv[0] --> exe
    argv[1] --> Original yuv path
    argv[2] --> Reconstructed yuv path
    argv[3] --> Width
    argv[4] --> Height
    argv[5] --> Chroma Subsampling
*/
int main()
{
    //VARIABLES
    int Width=432;
    int Height=240;
    int format=420;
    double chSubSamp[2]={0,0};
    long int OriginalFileSize;
    long int BlurFileSize;
    uint8_t *OriginalBuffer;
    uint8_t *BlurBuffer;
    size_t OriginalResult;
    size_t BlurResult;
    int FrameSize;
    char *OriginalFileName = "src\\right_432x240.yuv";
    char *BlurFileName = "src\\right_blurred_432x240.yuv";
    double Ypsnr=0;
    double Upsnr=0;
    double Vpsnr=0;
    double YpsnrAvg = 0;
    double UpsnrAvg = 0;
    double VpsnrAvg = 0;
    //Chroma Subsampling Selecting...
    if (format == 420){
        chSubSamp[0]=0.5;
        chSubSamp[1]=0.5;
    }
    else if (format == 444){
        chSubSamp[0]=1;
        chSubSamp[1]=1;
    }
    //Original file reading...
    FILE *OriginalFile;
    OriginalFile = fopen(OriginalFileName, "rb");
    if (OriginalFile == NULL){
        fputs("Original File Error.\n", stderr);
        exit(1);
    }
    // obtain file size:
    fseek(OriginalFile, 0, SEEK_END);
    OriginalFileSize = ftell(OriginalFile);
    rewind(OriginalFile);
    //allocate memory to contain the whole file:
    OriginalBuffer = (uint8_t*) malloc(OriginalFileSize+1);
    if (OriginalBuffer == NULL){
        fputs("Original Memory Error.\n",stderr);
        exit(2);
    }
    //copy file into the buffer:
    OriginalResult = fread(OriginalBuffer,1,OriginalFileSize,OriginalFile);
    if (OriginalResult != OriginalFileSize){
        fputs("Original Reading Error.\n", stderr);
        exit(3);
    }
    fclose(OriginalFile);


    //Blurred file reading...
    FILE *BlurFile;
    BlurFile = fopen(BlurFileName, "rb");
    if (BlurFile == NULL){
        fputs("Blurred File Error.\n", stderr);
        exit(1);
    }
    // obtain file size:
    fseek(BlurFile, 0, SEEK_END);
    BlurFileSize = ftell(BlurFile);
    rewind(BlurFile);
    //allocate memory to contain the whole file:
    BlurBuffer = (uint8_t*) malloc(BlurFileSize+1);
    if (BlurBuffer == NULL){
        fputs("Blurred Memory Error.\n",stderr);
        exit(2);
    }
    //copy file into the buffer:
    BlurResult = fread(BlurBuffer,1,BlurFileSize,BlurFile);
    if (BlurResult != BlurFileSize){
        fputs("Blurred Reading Error.\n", stderr);
        exit(3);
    }
    fclose(BlurFile);

    FrameSize = OriginalFileSize / (Width*Height*(1 + 2*chSubSamp[0]*chSubSamp[1]));

    printf("Original File Size: %d byte\n",OriginalFileSize);
    printf("Number of Frame: %d\n",FrameSize);
    printf("Chroma Subsampling: %d\n", format);

    //PSNR Calculating...
    long int loc=0;
    double fark = 0;
    double toplam =0;
    long int j=0;
    int peak = 255*255;
    long N=0;

    for (int i=0;i<FrameSize;i++){

        YpsnrAvg = YpsnrAvg + Ypsnr;
        UpsnrAvg = UpsnrAvg + Upsnr;
        VpsnrAvg = VpsnrAvg + Vpsnr;

        Ypsnr=0;
        Upsnr=0;
        Vpsnr=0;

        //PSNR of Y:
        N= Width*Height*1;
        j=0;
        toplam=0;
        for(j=0;j<N;j++){

            fark = (double)((int)OriginalBuffer[loc+j] - (int)BlurBuffer[loc+j]);
            toplam =toplam + (fark*fark);

        }
        toplam = toplam / N;
        Ypsnr = Ypsnr + 10*log10(peak/toplam);
        loc = loc + j;
        cout << fixed << setprecision(2) << setw(3) << i+1 << ". Frame: " <<  "Y-PSNR= "<< Ypsnr << " dB  |  ";
        //PSNR of U:
        j=0;
        toplam=0;
        N= Width*Height*chSubSamp[0]*chSubSamp[1];
        for(j=0;j<N;j++){
            fark = (double)((int)OriginalBuffer[loc+j] - (int)BlurBuffer[loc+j]);
            toplam =toplam + fark*fark;
        }
        toplam = toplam / N;
        Upsnr = Upsnr + 10*log10(peak/toplam);
        loc = loc + j;
        cout << "U-PSNR= " << Upsnr << " dB  |  ";
        //PSNR of V:

        j=0;
        toplam=0;
        N= Width*Height*chSubSamp[0]*chSubSamp[1];
        for(j=0;j<N;j++){
            fark = (double)((int)OriginalBuffer[loc+j] - (int)BlurBuffer[loc+j]);
            toplam =toplam + fark*fark;
        }

        toplam = toplam / N;
        Vpsnr = Vpsnr + 10*log10(peak/toplam);
        loc = loc + j;
        cout << "V-PSNR= " << Vpsnr << " dB" << endl;

    }
    YpsnrAvg = YpsnrAvg / FrameSize;
    UpsnrAvg = UpsnrAvg / FrameSize;
    VpsnrAvg = VpsnrAvg / FrameSize;
    cout << "Y-PSNR-Average= " << YpsnrAvg << " dB" << endl;
    cout << "U-PSNR-Average= " << UpsnrAvg << " dB" << endl;
    cout << "V-PSNR-Average= " << VpsnrAvg << " dB" << endl;
    free(OriginalBuffer);
    free(BlurBuffer);
    return 0;
}
