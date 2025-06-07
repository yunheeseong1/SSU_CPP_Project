#ifndef ADDEMPLOYEEDIALOG_H
#define ADDEMPLOYEEDIALOG_H

#include <QDialog>
#include "employee.h"

namespace Ui {
class AddEmployeeDialog;
}

// 직원 추가 또는 수정을 위한 대화상자 클래스
class AddEmployeeDialog : public QDialog
{
    Q_OBJECT

public:
    // 생성자 (새로운 직원을 추가할 때 사용)
    explicit AddEmployeeDialog(QWidget *parent = nullptr);
    // 생성자 (기존 직원 정보를 수정할 때 사용)
    explicit AddEmployeeDialog(const Employee& employeeToEdit, QWidget *parent = nullptr);
    ~AddEmployeeDialog();

    // 대화상자에 입력된 직원 정보를 반환하는 함수
    Employee getEmployeeDetails() const;
    // 대화상자의 창 제목을 설정하는 함수
    void setDialogTitle(const QString& title);
    // 직원 정보를 받아 대화상자의 필드를 채우는 함수
    void setEmployeeDetails(const Employee& employeeToEdit);


private:
    Ui::AddEmployeeDialog *ui; // UI 요소들을 관리하는 포인터
};

#endif
