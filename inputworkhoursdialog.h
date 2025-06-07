#ifndef INPUTWORKHOURSDIALOG_H
#define INPUTWORKHOURSDIALOG_H

#include <QDialog>
#include <QDate>
#include <QTime>
#include "worklog.h"

namespace Ui {
class InputWorkHoursDialog;
}

// 근무 시간을 입력, 수정, 삭제하기 위한 대화상자 클래스
class InputWorkHoursDialog : public QDialog
{
    Q_OBJECT

public:
    // 생성자 (새로운 근무 기록을 입력할 때 사용)
    explicit InputWorkHoursDialog(const QString &employeeName, int employeeIndex, const QDate &date, QWidget *parent = nullptr);
    // 생성자 (기존 근무 기록을 수정할 때 사용)
    InputWorkHoursDialog(const QString &employeeName, int employeeIndex,
                         const QDate &date, const WorkLog& existingLog, QWidget *parent = nullptr);
    ~InputWorkHoursDialog();

    // 대화상자에 입력된 근무 시간 정보를 반환
    WorkLog getWorkLog() const;
    // 사용자가 '삭제' 버튼을 눌렀는지 여부를 반환
    bool isDeleteRequested() const { return m_deleteRequested; }

private slots:
    // '삭제' 버튼 클릭 시 실행
    void onDeleteButtonClicked();


private:
    Ui::InputWorkHoursDialog *ui; // UI 요소 관리 포인터
    bool m_isEditMode; // 수정 모드 여부
    bool m_deleteRequested; // 삭제 요청 여부
    WorkLog m_originalWorkLog; // 수정 모드일 때 원본 데이터
    int m_employeeIndex; // 대상 직원의 ID
    QDate m_date; // 대상 근무 날짜
};

#endif // INPUTWORKHOURSDIALOG_H
