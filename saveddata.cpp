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
            shelterName = in.readLine();
            shelterPhone = in.readLine();
            shelterAddress = in.readLine();
            shelterAddress2 = in.readLine();
            shelterPostalCode = in.readLine();
            shelterCity = in.readLine();

            file.close();

            return true;
        }

        file.close();
    }

    file.open(QIODevice::ReadWrite); // (Re)create file if not valid
    file.close();

    return false;
}

QStringList SavedData::GetShelterInfos(){
    return {shelterName, shelterPhone, shelterAddress, shelterAddress2, shelterPostalCode, shelterCity};
}
