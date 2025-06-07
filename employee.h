#ifndef EMPLOYEE_H
#define EMPLOYEE_H

#include <QString>
#include <QColor>
#include <QJsonObject>
#include <QJsonValue>

// 직원 한 명의 정보를 담는 데이터 클래스
class Employee
{
public:
    // 생성자: 직원 객체를 초기화
    Employee(int id = -1, const QString &name = "", int hourlyWage = 0, const QString &account = "", const QColor &color = Qt::black);

    // --- 정보 가져오기 (Getter) ---
    int getId() const;
    QString getName() const;
    int getHourlyWage() const;
    QString getBankAccount() const;

    // --- 정보 설정하기 (Setter) ---
    void setName(const QString &name);
    void setHourlyWage(int wage);
    void setBankAccount(const QString &account);
    void internalSetId(int id); // DataManager가 내부적으로 ID를 할당할 때 사용

    // --- JSON 변환 함수 (파일 저장/불러오기용) ---
    QJsonObject toJson() const; // 객체 정보를 JSON으로 변환
    static Employee fromJson(const QJsonObject &json); // JSON 데이터로 객체 생성

private:
    int m_id; // 직원 고유 ID
    QString m_name; // 이름
    int m_hourlyWage; // 시급
    QString m_bankAccount; // 계좌번호
};

#endif // EMPLOYEE_H
