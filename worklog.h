#ifndef WORKLOG_H
#define WORKLOG_H

#include <QDate>
#include <QTime>
#include <QJsonObject>

// 근무 기록 한 건의 정보를 담는 데이터 클래스
class WorkLog
{
public:
    // 생성자: 근무 기록 객체를 초기화
    WorkLog(int employeeId = -1,
            const QDate &date = QDate(),
            const QTime &startTime = QTime(),
            const QTime &endTime = QTime());

    // --- 정보 가져오기/설정하기 (Getter/Setter) ---
    int getEmployeeId() const;
    void setEmployeeId(int id);
    QDate getDate() const;
    void setDate(const QDate &date);
    QTime getStartTime() const;
    void setStartTime(const QTime &time);
    QTime getEndTime() const;
    void setEndTime(const QTime &time);
    // 출근/퇴근 시간으로 실제 근무 시간을 계산해서 반환
    double getHoursWorked() const;

    // --- JSON 변환 함수 (파일 저장/불러오기용) ---
    // 객체 정보를 JSON으로 변환
    QJsonObject toJson() const;
    // JSON 데이터로 객체 생성
    static WorkLog fromJson(const QJsonObject &json);

private:
    int m_employeeId; // 직원 고유 ID
    QDate m_date; // 근무 날짜
    QTime m_startTime; // 근무 시작 시간
    QTime m_endTime; // 근무 종료 시간
};

#endif
