#include "datamanager.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>
#include <algorithm> // std::max 사용을 위해 (loadData에서)

DataManager::DataManager() : m_nextEmployeeId(1) // m_nextEmployeeId를 1로 초기화
{

}

// Employee 객체를 받아서 ID와 색상을 할당하고 리스트에 추가
// 파라미터를 Employee& employee로 변경하여 전달된 객체에 ID를 직접 설정합니다.
// 또는 const Employee&로 받고 내부에서 복사본을 만들어 ID와 색상을 설정 후 저장할 수도 있습니다.
// 여기서는 Employee&를 사용합니다. (호출부에서 Employee emp = ...; addEmployee(emp); 형태로 사용)
void DataManager::addEmployee(Employee &employee) // Employee&로 변경
{
    employee.internalSetId(m_nextEmployeeId++); // Employee 객체에 새 ID 할당 및 m_nextEmployeeId 증가


    m_employees.append(employee); // ID와 색상이 설정된 직원 객체를 리스트에 추가
}

const QList<Employee>& DataManager::getEmployees() const
{
    return m_employees;
}

Employee DataManager::getEmployeeById(int employeeId) const
{
    for (const Employee &emp : m_employees) {
        if (emp.getId() == employeeId) {
            return emp;
        }
    }
    qWarning() << "Employee with ID" << employeeId << "not found.";
    return Employee(); // ID가 -1인 기본 Employee 객체 반환
}

bool DataManager::updateEmployeeById(int employeeId, const Employee &updatedEmployeeInfo)
{
    for (int i = 0; i < m_employees.size(); ++i) {
        if (m_employees[i].getId() == employeeId) {
            // ID는 변경하지 않고, 다른 정보만 업데이트
            // updatedEmployeeInfo에도 ID가 있겠지만, employeeId 파라미터로 찾은 객체를 수정
            m_employees[i].setName(updatedEmployeeInfo.getName());
            m_employees[i].setHourlyWage(updatedEmployeeInfo.getHourlyWage());
            m_employees[i].setBankAccount(updatedEmployeeInfo.getBankAccount());
            qDebug() << "Employee with ID" << employeeId << "updated.";
            return true;
        }
    }
    qWarning() << "Failed to update. Employee with ID" << employeeId << "not found.";
    return false;
}

bool DataManager::deleteEmployeeById(int employeeId)
{
    bool employeeRemoved = false;
    // 1. 직원 목록에서 해당 ID의 직원 찾아 삭제
    for (int i = 0; i < m_employees.size(); ++i) {
        if (m_employees.at(i).getId() == employeeId) {
            m_employees.removeAt(i);
            employeeRemoved = true;
            qDebug() << "Employee with ID" << employeeId << "removed from m_employees.";
            break; // 찾았으면 루프 종료
        }
    }

    if (!employeeRemoved) {
        qWarning() << "Failed to delete. Employee with ID" << employeeId << "not found in m_employees.";
        return false;
    }

    // 2. 근무 기록 목록(m_workLogs)에서 해당 employeeId를 가진 모든 WorkLog 삭제
    // QMutableListIterator를 사용하면 반복 중 안전하게 항목을 삭제할 수 있습니다.
    int logsRemovedCount = 0;
    QMutableListIterator<WorkLog> iter(m_workLogs);
    while (iter.hasNext()) {
        // WorkLog 클래스에 getEmployeeId() 함수가 구현되어 있다고 가정합니다.
        if (iter.next().getEmployeeId() == employeeId) {
            iter.remove();
            logsRemovedCount++;
        }
    }
    qDebug() << logsRemovedCount << "worklog(s) for employee ID" << employeeId << "deleted.";
    return true;
}


// --- WorkLog 관련 함수들 ---
void DataManager::addWorkLog(const WorkLog &log)
{
    // WorkLog 객체는 이미 employeeId를 가지고 생성되었다고 가정합니다.
    m_workLogs.append(log);
    qDebug() << "Worklog added for employee ID:" << log.getEmployeeId() // getEmployeeIndex() 대신 getEmployeeId()
             << "on date:" << log.getDate().toString("yyyy-MM-dd")
             << "for hours:" << log.getHoursWorked();
}

QList<WorkLog> DataManager::getWorkLogsForEmployeeOnDate(int employeeId, const QDate &date) const
{
    QList<WorkLog> resultLogs;
    for (const WorkLog &log : m_workLogs) {
        if (log.getEmployeeId() == employeeId && log.getDate() == date) { // getEmployeeIndex() 대신 getEmployeeId()
            resultLogs.append(log);
        }
    }
    return resultLogs;
}

QList<WorkLog> DataManager::getWorkLogsForDate(const QDate &date) const
{
    QList<WorkLog> resultLogs;
    for (const WorkLog &log : m_workLogs) {
        if (log.getDate() == date) {
            resultLogs.append(log);
        }
    }
    return resultLogs;
}

QList<WorkLog> DataManager::getWorkLogsForEmployeeForMonth(int employeeId, int year, int month) const
{
    QList<WorkLog> resultLogs;
    for (const WorkLog &log : m_workLogs) {
        if (log.getEmployeeId() == employeeId && // getEmployeeIndex() 대신 getEmployeeId()
            log.getDate().year() == year &&
            log.getDate().month() == month) {
            resultLogs.append(log);
        }
    }
    return resultLogs;
}

// 특정 날짜의 특정 직원 근무 기록 모두 삭제
bool DataManager::deleteWorkLogsForEmployeeOnDate(int employeeId, const QDate& date)
{
    bool changed = false;
    QMutableListIterator<WorkLog> i(m_workLogs);
    while (i.hasNext()) {
        const WorkLog& log = i.next();
        if (log.getEmployeeId() == employeeId && log.getDate() == date) { // getEmployeeIndex() 대신 getEmployeeId()
            i.remove();
            changed = true;
        }
    }
    if (changed) {
        qDebug() << "Worklogs for employee ID" << employeeId << "on date" << date.toString("yyyy-MM-dd") << "deleted.";
    }
    return changed;
}


// --- 데이터 저장/불러오기 함수 ---
bool DataManager::saveData(const QString &filename) const
{
    QFile saveFile(filename);
    if (!saveFile.open(QIODevice::WriteOnly)) {
        qWarning("Couldn't open save file for writing.");
        return false;
    }

    QJsonObject rootObject;
    rootObject["nextEmployeeId"] = m_nextEmployeeId; // m_nextEmployeeId 저장

    QJsonArray employeeArray;
    for (const Employee &emp : m_employees) {
        employeeArray.append(emp.toJson()); // Employee::toJson()이 ID를 포함해야 함
    }
    rootObject["employees"] = employeeArray;

    QJsonArray worklogArray;
    for (const WorkLog &log : m_workLogs) {
        worklogArray.append(log.toJson()); // WorkLog::toJson()이 employeeId를 포함해야 함
    }
    rootObject["worklogs"] = worklogArray;

    QJsonDocument saveDoc(rootObject);
    saveFile.write(saveDoc.toJson()); // 텍스트 기반 JSON으로 저장
    qDebug() << "Data saved to" << filename << ". NextEmployeeId:" << m_nextEmployeeId
             << "Employees:" << m_employees.size() << "Worklogs:" << m_workLogs.size();
    return true;
}

bool DataManager::loadData(const QString &filename)
{
    QFile loadFile(filename);
    if (!loadFile.open(QIODevice::ReadOnly)) {
        qWarning("Couldn't open save file for reading. No existing data or file not found.");
        return false;
    }

    QByteArray saveData = loadFile.readAll();
    QJsonDocument loadDoc(QJsonDocument::fromJson(saveData));

    if (loadDoc.isNull() || !loadDoc.isObject()) {
        qWarning("Failed to create QJsonDocument from file or it's not an object.");
        return false;
    }

    QJsonObject rootObject = loadDoc.object();

    // m_nextEmployeeId 불러오기 (Employee 목록보다 먼저 또는 직원 ID 최대값 계산 후 설정)
    m_nextEmployeeId = rootObject.value("nextEmployeeId").toInt(1); // 파일에 없으면 기본값 1

    m_employees.clear();
    if (rootObject.contains("employees") && rootObject["employees"].isArray()) {
        QJsonArray employeeArray = rootObject["employees"].toArray();
        int maxIdLoaded = 0;
        for (int i = 0; i < employeeArray.size(); ++i) {
            QJsonObject empObject = employeeArray[i].toObject();
            Employee emp = Employee::fromJson(empObject); // Employee::fromJson()이 ID를 처리해야 함
            m_employees.append(emp);
            if (emp.getId() > maxIdLoaded) {
                maxIdLoaded = emp.getId();
            }
        }
        // 로드된 직원 ID 중 가장 큰 값보다 m_nextEmployeeId가 작거나 같으면, 충돌 방지를 위해 업데이트
        if (m_nextEmployeeId <= maxIdLoaded) {
            m_nextEmployeeId = maxIdLoaded + 1;
        }
    }


    m_workLogs.clear();
    if (rootObject.contains("worklogs") && rootObject["worklogs"].isArray()) {
        QJsonArray worklogArray = rootObject["worklogs"].toArray();
        for (int i = 0; i < worklogArray.size(); ++i) {
            QJsonObject logObject = worklogArray[i].toObject();
            m_workLogs.append(WorkLog::fromJson(logObject)); // WorkLog::fromJson()이 employeeId를 처리해야 함
        }
    }
    qDebug() << "Data loaded from" << filename << ". NextEmployeeId:" << m_nextEmployeeId
             << "Employees count:" << m_employees.size() << "Worklogs count:" << m_workLogs.size();
    return true;
}

WorkLog DataManager::getWorkLogByEmployeeAndDate(int employeeId, const QDate& date) const
{
    for (const WorkLog& log : m_workLogs) {
        if (log.getEmployeeId() == employeeId && log.getDate() == date) {
            return log;
        }
    }
    return WorkLog(-1, QDate(), QTime(), QTime()); // 찾지 못한 경우
}

bool DataManager::updateWorkLog(const WorkLog& oldLog, const WorkLog& newLog)
{
    for (int i = 0; i < m_workLogs.size(); ++i) {
        if (m_workLogs[i].getEmployeeId() == oldLog.getEmployeeId() &&
            m_workLogs[i].getDate() == oldLog.getDate()) {
            m_workLogs[i] = newLog;
            return true;
        }
    }
    return false;
}
const QList<WorkLog>& DataManager::getWorkLogs() const
{
    return m_workLogs;
}

bool DataManager::deleteWorkLog(int employeeId, const QDate& date)
{
    for (int i = 0; i < m_workLogs.size(); ++i) {
        if (m_workLogs[i].getEmployeeId() == employeeId &&
            m_workLogs[i].getDate() == date) {
            m_workLogs.removeAt(i);
            return true;
        }
    }
    return false;
}
