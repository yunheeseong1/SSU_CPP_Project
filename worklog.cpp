#include "worklog.h"

// 생성자: 근무 기록 객체의 멤버 변수들을 초기화
WorkLog::WorkLog(int employeeId, const QDate &date, const QTime &startTime, const QTime &endTime)
    : m_employeeId(employeeId), m_date(date), m_startTime(startTime), m_endTime(endTime)
{
}

// 직원 ID를 반환
int WorkLog::getEmployeeId() const // 이름 변경
{
    return m_employeeId;
}

// 직원 ID를 설정
void WorkLog::setEmployeeId(int id) // 이름 변경
{
    m_employeeId = id;
}

// --- 나머지 Getter/Setter 함수들 ---
QDate WorkLog::getDate() const { return m_date; }
void WorkLog::setDate(const QDate &date) { m_date = date; }
QTime WorkLog::getStartTime() const { return m_startTime; }
void WorkLog::setStartTime(const QTime &time) { m_startTime = time; }
QTime WorkLog::getEndTime() const { return m_endTime; }
void WorkLog::setEndTime(const QTime &time) { m_endTime = time; }

// 실제 근무 시간을 시간 단위로 계산 (자정 넘어가는 경우 처리 포함)
double WorkLog::getHoursWorked() const {
    if (!m_startTime.isValid() || !m_endTime.isValid()) return 0.0;
    // 시작 시간부터 종료 시간까지의 초(second) 차이를 계산
    int seconds = m_startTime.secsTo(m_endTime);
    // 야간 근무 등으로 종료 시간이 다음 날이라 초가 음수이면, 하루(24시간)만큼 더해줌
    if (seconds < 0) seconds += 24 * 60 * 60;
    // 계산된 초를 3600으로 나누어 시간 단위로 변환
    return static_cast<double>(seconds) / 3600.0;
}


// 객체 정보를 JSON으로 변환 (파일 저장용)
QJsonObject WorkLog::toJson() const
{
    QJsonObject json;
    json["employeeId"] = m_employeeId;
    json["date"] = m_date.toString(Qt::ISODate);
    json["startTime"] = m_startTime.toString("HH:mm:ss");
    json["endTime"] = m_endTime.toString("HH:mm:ss");
    return json;
}

// JSON 데이터로 객체를 생성 (파일 불러오기용)
WorkLog WorkLog::fromJson(const QJsonObject &json)
{
    int employeeId = json["employeeId"].toInt(-1);
    QDate date = QDate::fromString(json["date"].toString(), Qt::ISODate);
    QTime startTime = QTime::fromString(json["startTime"].toString(), "HH:mm:ss");
    QTime endTime = QTime::fromString(json["endTime"].toString(), "HH:mm:ss");

    // 읽어온 정보로 새로운 WorkLog 객체를 생성하여 반환
    return WorkLog(employeeId, date, startTime, endTime);
}
