#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>
#include "fstream"

using namespace std;
int main(int argc, const char **argv)
{
    string fname =argv[1];
    string::size_type pAtPath = fname.find_last_of('/');
    string filePath = fname.substr(0, pAtPath + 1);
    string fileFullName = fname.substr(pAtPath + 1);
    string::size_type pAtExt = fileFullName.find_last_of('.');   // Find extension point
    string fileBaseName = fileFullName.substr(0, pAtExt);

    string currentDirectory=filePath;
    string filename=fileBaseName;


    Pix *image = pixRead(argv[1]);
    tesseract::TessBaseAPI *api = new tesseract::TessBaseAPI();
    api->Init(NULL, "eng");
    api->SetImage(image);
    api->Recognize(0);
    tesseract::ResultIterator* ri = api->GetIterator();
    tesseract::PageIteratorLevel level = tesseract::RIL_TEXTLINE;

    string newDirectory=currentDirectory+filename;
    string command="mkdir -p " + newDirectory;
    system(command.c_str());

    //  newDirectory=newDirectory+'/'+filename

    int a=0;
    if (ri != 0) {
        do {
            const char* word = ri->GetUTF8Text(level);
            float conf = ri->Confidence(level);
            int x1, y1, x2, y2;
            ri->BoundingBox(level, &x1, &y1, &x2, &y2);
            printf("word: '%s';  \tconf: %.2f; BoundingBox: %d,%d,%d,%d;\n",
                   word, conf, x1, y1, x2, y2);

            ofstream file(newDirectory+"/line-"+ to_string(a) +".txt");
            file << word << endl;
            delete[] word;
            file.close();
            a++;
        } while (ri->Next(level));
    }
}