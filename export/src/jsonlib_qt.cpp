#include <rgen/private/jsonlib_qt.h>

template <>
RGEN_DECL_EXPORT void fromTypeToJson(rgen::json& json, const QString& s)
{
    fromTypeToJson(json, s.toStdString());
}

template <>
RGEN_DECL_EXPORT void fromJsonToType(const rgen::json& json, QString& s)
{
    s = QString::fromStdString(json.get<std::string>());
}

template <>
RGEN_DECL_EXPORT void fromTypeToJson(rgen::json& json, const QUuid& uuid)
{
    json = uuid.toString(QUuid::WithoutBraces).toStdString();
}

template <>
RGEN_DECL_EXPORT void fromJsonToType(const rgen::json& json, QUuid& uuid)
{
    uuid = QUuid(QString::fromStdString(json.get<std::string>()));
}

template <>
RGEN_DECL_EXPORT void fromJsonToType(const rgen::json& json, QStringList& v)
{
    v.reserve(json.size());
    for (const auto& item : json)
    {
        QString value;
        fromJsonToType(item, value);
        v.push_back(value);
    }
}

template <>
RGEN_DECL_EXPORT void fromTypeToJson(rgen::json& json, const QStringList& val)
{
    rgen::_details_std::containerToJson(json, val);
}

template <>
RGEN_DECL_EXPORT void fromTypeToJson(rgen::json& json, const QByteArray& s)
{
    json = rgen::json::array();

    for (qsizetype i = 0; i < s.size(); ++i)
    {
        json.push_back(s.at(i));
    }
}

template <>
RGEN_DECL_EXPORT void fromJsonToType(const rgen::json& json, QByteArray& s)
{
    s.clear();

    for (const auto& item : json)
    {
        s.append(static_cast<char>(item.get<int64_t>()));
    }
}
