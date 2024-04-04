#include "saveddata.h"

SavedData::SavedData()
{

}

void SavedData::Save(){

}

bool SavedData::Load(){
    QFile file("save");
    if (file.open(QIODevice::ReadOnly)) {
        QTextStream in(&file);

        accessHash = in.readLine();

        if(!accessHash.isEmpty()){
            file.close();

            return true;
        }

        file.close();
    }

    file.open(QIODevice::ReadWrite); // (Re)create file if not valid
    file.close();

    return false;
}
