#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include <QList>
#include <QColor>
#include <QString>
#include "employee.h"
#include "worklog.h"

// 프로그램의 모든 데이터(직원, 근무 기록)를 관리하는 클래스
class DataManager
{
public:
    DataManager();

    // --- 직원 관리 함수 ---
    void addEmployee(Employee &employee); // 새 직원 추가
    const QList<Employee>& getEmployees() const; // 모든 직원 목록 반환
    bool deleteEmployeeById(int employeeId); // ID로 직원 삭제
    bool updateEmployeeById(int employeeId, const Employee &updatedEmployeeInfo); // ID로 직원 정보 수정
    Employee getEmployeeById(int employeeId) const; // ID로 특정 직원 정보 조회

    // --- 근무 기록 관리 함수 ---
    void addWorkLog(const WorkLog &log); // 새 근무 기록 추가
    QList<WorkLog> getWorkLogsForEmployeeOnDate(int employeeId, const QDate &date) const; // 특정 직원의 특정 날짜 근무 기록 조회
    QList<WorkLog> getWorkLogsForDate(const QDate &date) const; // 특정 날짜의 모든 근무 기록 조회
    QList<WorkLog> getWorkLogsForEmployeeForMonth(int employeeId, int year, int month) const; // 특정 직원의 특정 월 근무 기록 조회
    bool deleteWorkLogsForEmployeeOnDate(int employeeId, const QDate& date); // 특정 직원의 특정 날짜 근무 기록 삭제

    // --- 데이터 저장/불러오기 ---
    bool saveData(const QString &filename) const; // 모든 데이터를 파일에 저장
    bool loadData(const QString &filename); // 파일에서 데이터 불러오기

    // --- 개별 근무 기록 관리 ---
    WorkLog getWorkLogByEmployeeAndDate(int employeeId, const QDate& date) const; // 특정 직원의 특정 날짜 근무 기록 찾기
    bool updateWorkLog(const WorkLog& oldLog, const WorkLog& newLog); // 근무 기록 수정
    bool deleteWorkLog(int employeeId, const QDate& date); // 근무 기록 삭제
    const QList<WorkLog>& getWorkLogs() const; // 모든 근무 기록 목록 반환

private:
    QList<Employee> m_employees; // 직원 목록
    QList<WorkLog> m_workLogs;   // 근무 기록 목록
    QList<QColor> m_employeeColorCycle; // 직원별 색상 (현재 미사용)
    int m_nextEmployeeId;        // 다음 직원에게 할당할 ID
};

#endif // DATAMANAGER_H
