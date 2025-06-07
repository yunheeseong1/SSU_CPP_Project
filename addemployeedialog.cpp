#include "addemployeedialog.h"
#include "ui_addemployeedialog.h"
#include "employee.h"

// 기본 생성자 (새로운 직원을 추가할 때 호출)
AddEmployeeDialog::AddEmployeeDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddEmployeeDialog)
{
    ui->setupUi(this);
    setWindowTitle("직원 추가"); // 기본 창 제목 설정
}

// 직원 정보 수정을 위한 생성자 (기존 정보를 받아 UI에 채워넣음)
AddEmployeeDialog::AddEmployeeDialog(const Employee& employeeToEdit, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddEmployeeDialog)
{
    ui->setupUi(this);
    setWindowTitle("직원 정보 수정");
    setEmployeeDetails(employeeToEdit); // 전달받은 정보로 UI 필드를 채움
}

// 소멸자 (UI 메모리 해제)
AddEmployeeDialog::~AddEmployeeDialog()
{
    delete ui;
}

// UI에 입력된 정보를 Employee 객체로 만들어 반환
Employee AddEmployeeDialog::getEmployeeDetails() const
{
    // 각 입력 필드에서 텍스트를 가져옴
    QString name = ui->nameLineEdit->text();
    int hourlyWage = ui->wageLineEdit->text().toInt();
    QString account = ui->accountLineEdit->text();
    // 새 직원의 ID는 DataManager가 나중에 할당하므로 임시 ID(-1)를 사용
    int temporaryId = -1;

    // 가져온 정보로 Employee 객체를 생성하여 반환
    return Employee(temporaryId, name, hourlyWage, account);
}

// 대화상자의 창 제목을 설정
void AddEmployeeDialog::setDialogTitle(const QString &title)
{
    setWindowTitle(title);
}

// 직원 정보를 받아와 UI의 각 필드에 채워넣음
void AddEmployeeDialog::setEmployeeDetails(const Employee &employeeToEdit)
{
    ui->nameLineEdit->setText(employeeToEdit.getName());
    ui->wageLineEdit->setText(QString::number(employeeToEdit.getHourlyWage()));
    ui->accountLineEdit->setText(employeeToEdit.getBankAccount());
}
