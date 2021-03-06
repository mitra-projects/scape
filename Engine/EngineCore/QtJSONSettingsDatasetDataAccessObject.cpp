#include "ScapeEngineStableHeaders.h"
#include "EngineCore/QtJSONSettingsDatasetDataAccessObject.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

namespace ScapeEngine
{

QtJSONSettingsDatasetDataAccessObject::QtJSONSettingsDatasetDataAccessObject(std::string fileName)
    : mFileHelper(fileName)
{
}

QtJSONSettingsDatasetDataAccessObject::~QtJSONSettingsDatasetDataAccessObject() {}

const SettingsDataset::SectionMapStruct
QtJSONSettingsDatasetDataAccessObject::getSettingsDataset(const std::string& datasetName)
{
    SettingsDataset::SectionMapStruct ret;

    QJsonParseError errorStruct;
    errorStruct.error = QJsonParseError::NoError;

    QJsonDocument jsonDoc = mFileHelper.readJSONFile(errorStruct);
    if (errorStruct.error == QJsonParseError::NoError)
    {
        if (jsonDoc.isObject())
        {
            QJsonObject rootObject = jsonDoc.object();

            if (rootObject.contains(QString(datasetName.c_str())))
            {
                QJsonValue datasetValue = rootObject[QString(datasetName.c_str())];
                if (datasetValue.isObject())
                {
                    QJsonObject datasetRoot = datasetValue.toObject();
                    recurseObjects(datasetRoot, ret, std::vector<std::string>());
                }
            }
        }
    }

    return ret;
}

void QtJSONSettingsDatasetDataAccessObject::updateSettingsDataset(
    const SettingsDataset::SectionMapStruct& settingsDataset, const std::string& datasetName)
{
    QJsonParseError errorStruct;
    errorStruct.error = QJsonParseError::NoError;

    QJsonDocument jsonDoc = mFileHelper.readJSONFile(errorStruct);

    if (errorStruct.error == QJsonParseError::NoError)
    {
        if (jsonDoc.isObject())
        {
            QJsonObject rootObject = jsonDoc.object();

            QJsonObject::iterator it = rootObject.find(QString(datasetName.c_str()));
            if (it != rootObject.end())
            {
                rootObject.erase(it);
            }

            QJsonObject datasetRoot = QJsonObject();

            for (SettingsDataset::SectionMapStruct::const_iterator secIt = settingsDataset.begin();
                 secIt != settingsDataset.end(); secIt++)
            {
                const std::string& sectionName = secIt->first;
                QJsonObject sectionObject = QJsonObject();
                for (SettingsDataset::SubsectionMapStruct::const_iterator subSecIt = secIt->second.begin();
                     subSecIt != secIt->second.end(); subSecIt++)
                {
                    const std::string& subSectionName = subSecIt->first;
                    QJsonObject subSectionObject = QJsonObject();
                    for (SettingsDataset::KeyMapStruct::const_iterator valuesIt = subSecIt->second.begin();
                         valuesIt != subSecIt->second.end(); valuesIt++)
                    {
                        const std::string& key = valuesIt->first;
                        const std::string& value = valuesIt->second;
                        if (sectionName == "")
                        {
                            datasetRoot.insert(QString(key.c_str()), QString(value.c_str()));
                        }
                        else if (subSectionName == "")
                        {
                            sectionObject.insert(QString(key.c_str()), QString(value.c_str()));
                        }
                        else
                        {
                            subSectionObject.insert(QString(key.c_str()), QString(value.c_str()));
                        }
                    }
                    if (subSectionName != "")
                    {
                        sectionObject.insert(QString(subSectionName.c_str()), subSectionObject);
                    }
                }
                if (sectionName != "")
                {
                    datasetRoot.insert(QString(sectionName.c_str()), sectionObject);
                }
            }

            rootObject.insert(QString(datasetName.c_str()), datasetRoot);

            jsonDoc = QJsonDocument(rootObject);
        }
        mFileHelper.writeJSONFile(jsonDoc);
    }
}

void QtJSONSettingsDatasetDataAccessObject::recurseObjects(QJsonObject& parent,
                                                           SettingsDataset::SectionMapStruct& root,
                                                           std::vector<std::string> path)
{
    for (QJsonObject::iterator parentIt = parent.begin(); parentIt != parent.end(); parentIt++)
    {
        QJsonValue currentValue = parentIt.value();
        if (currentValue.isString())
        {
            QString childValue = currentValue.toString();
            std::string childValueStr = childValue.toStdString();

            QString childKey = parentIt.key();
            std::string childKeyStr = childKey.toStdString();

            std::string sectionName = "";
            std::string subSectionName = "";
            std::string keyName = childKeyStr;

            if (path.size() == 1)
            {
                sectionName = path[0];
            }
            else if (path.size() == 2)
            {
                sectionName = path[0];
                subSectionName = path[1];
            }

            SettingsDataset::SectionMapStruct::iterator sectionIt = root.find(sectionName);
            if (sectionIt == root.end())
            {
                sectionIt =
                    root.insert(root.end(), SettingsDataset::SectionMapStruct::value_type(
                                                sectionName, SettingsDataset::SubsectionMapStruct()));
            }

            SettingsDataset::SubsectionMapStruct& section = sectionIt->second;

            SettingsDataset::SubsectionMapStruct::iterator subSectionIt = section.find(subSectionName);
            if (subSectionIt == section.end())
            {
                subSectionIt =
                    section.insert(section.end(), SettingsDataset::SubsectionMapStruct::value_type(
                                                      subSectionName, SettingsDataset::KeyMapStruct()));
            }

            SettingsDataset::KeyMapStruct& subSection = subSectionIt->second;

            SettingsDataset::KeyMapStruct::iterator keysIt = subSection.find(keyName);
            if (keysIt == subSection.end())
            {
                subSection.insert(subSection.end(),
                                  SettingsDataset::KeyMapStruct::value_type(keyName, childValueStr));
            }
        }
        else if (currentValue.isObject())
        {
            if (path.size() < 3)
            {
                std::vector<std::string> newPath = path;
                newPath.push_back(parentIt.key().toStdString());

                QJsonObject childObject = currentValue.toObject();
                recurseObjects(childObject, root, newPath);
            }
        }
    }
}
}
