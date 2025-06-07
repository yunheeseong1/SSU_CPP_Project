#include "employee.h"
#include "employee.h"

// 생성자: 직원 객체의 멤버 변수들을 초기화
Employee::Employee(int id, const QString &name, int hourlyWage, const QString &account, const QColor &color)
    : m_id(id), // m_id 멤버 초기화 추가!
    m_name(name),
    m_hourlyWage(hourlyWage),
    m_bankAccount(account)
{

}

// --- Getter 함수들: 멤버 변수의 값을 반환 ---
QString Employee::getName() const
{
    return m_name;
}

int Employee::getHourlyWage() const
{
    return m_hourlyWage;
}

QString Employee::getBankAccount() const
{
    return m_bankAccount;
}


// --- Setter 함수들: 멤버 변수의 값을 설정 ---
void Employee::setName(const QString &name)
{
    m_name = name;
}

void Employee::setHourlyWage(int wage)
{
    m_hourlyWage = wage;
}

void Employee::setBankAccount(const QString &account)
{
    m_bankAccount = account;
}



int Employee::getId() const
{
    return m_id;
}

// DataManager가 내부적으로 ID를 할당할 때 사용하는 함수
void Employee::internalSetId(int id)
{
    m_id = id;
}


// 객체 정보를 JSON으로 변환 (파일 저장용)
QJsonObject Employee::toJson() const
{
    QJsonObject json;
    json["id"] = m_id; // ID 저장
    json["name"] = m_name;
    json["hourlyWage"] = m_hourlyWage;
    json["bankAccount"] = m_bankAccount;
    return json;
}

// JSON 데이터로부터 Employee 객체를 생성 (파일 불러오기용)
Employee Employee::fromJson(const QJsonObject &json)
{
    int id = json["id"].toInt(-1); // ID 불러오기 (없으면 -1)
    QString name = json["name"].toString();
    int hourlyWage = json["hourlyWage"].toInt();
    QString bankAccount = json["bankAccount"].toString();

    // 읽어온 정보로 새로운 Employee 객체를 생성하여 반환
    return Employee(id, name, hourlyWage, bankAccount);
}
