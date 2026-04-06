#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTableWidget>
#include <QTextEdit>
#include <QProgressBar>
#include <QTimer>
#include <QProcess>
#include <QFileDialog>
#include <QMessageBox>
#include <QFrame>
#include <QScrollArea>
#include <QHeaderView>
#include <QPainter>
#include <QPainterPath>
#include <QPropertyAnimation>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QFile>
#include <QDir>
#include <vector>
#include <string>

// ═══════════════════════════════════════════════════════════════
//  DESIGN TOKENS
// ═══════════════════════════════════════════════════════════════
namespace Theme {
    const QString BG_PRIMARY   = "#0A0B1E";
    const QString BG_SIDEBAR   = "#060714";
    const QString BG_CARD      = "#111228";
    const QString BG_CARD_HOVER= "#1a1b3a";
    const QString BG_SURFACE   = "#1e1f3a";
    const QString BG_INPUT     = "#0d0e22";
    const QString BORDER       = "#1e2044";
    const QString TEXT_PRIMARY = "#e2e8f0";
    const QString TEXT_SECONDARY="#8892b0";
    const QString TEXT_DIM     = "#4a5568";
    const QString ACCENT_BLUE  = "#3b82f6";
    const QString ACCENT_CYAN  = "#06b6d4";
    const QString ACCENT_GREEN = "#10b981";
    const QString ACCENT_RED   = "#ef4444";
    const QString ACCENT_AMBER = "#f59e0b";
    const QString ACCENT_PURPLE= "#a855f7";
}

// ═══════════════════════════════════════════════════════════════
//  RISK GAUGE WIDGET (Custom QPainter)
// ═══════════════════════════════════════════════════════════════
class RiskGauge : public QWidget {
    Q_OBJECT
public:
    explicit RiskGauge(QWidget* parent = nullptr);
    void setValue(double val);
    double value() const { return m_value; }
protected:
    void paintEvent(QPaintEvent* event) override;
private:
    double m_value = 0;
};

// ═══════════════════════════════════════════════════════════════
//  SEVERITY BAR WIDGET
// ═══════════════════════════════════════════════════════════════
class SeverityBar : public QWidget {
    Q_OBJECT
public:
    explicit SeverityBar(QWidget* parent = nullptr);
    void setValues(int low, int med, int high, int crit);
protected:
    void paintEvent(QPaintEvent* event) override;
private:
    int m_low=0, m_med=0, m_high=0, m_crit=0;
};

// ═══════════════════════════════════════════════════════════════
//  STAT CARD WIDGET
// ═══════════════════════════════════════════════════════════════
class StatCard : public QFrame {
    Q_OBJECT
public:
    StatCard(const QString& title, const QString& value, const QString& color, QWidget* parent = nullptr);
    void setValue(const QString& v);
    void setValueColor(const QString& color);
private:
    QLabel* m_titleLbl;
    QLabel* m_valueLbl;
};

// ═══════════════════════════════════════════════════════════════
//  NAV BUTTON
// ═══════════════════════════════════════════════════════════════
class NavButton : public QPushButton {
    Q_OBJECT
public:
    NavButton(const QString& text, const QString& iconChar, QWidget* parent = nullptr);
};

// ═══════════════════════════════════════════════════════════════
//  EVENT STRUCT
// ═══════════════════════════════════════════════════════════════
struct LogEvent {
    int line = 0;
    QString timestamp;
    QString severity;
    QString action;
    QString user;
    QString ip_address;
    QString process;
    QString mitre_technique;
    QString raw_log;
    bool is_suspicious = false;
};

// ═══════════════════════════════════════════════════════════════
//  MAIN WINDOW
// ═══════════════════════════════════════════════════════════════
class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);

private slots:
    void switchPage(int idx);
    void onScan();
    void onToggleTrack();
    void onAIReport();
    void onVerifyChain();
    void onExport();
    void onRefreshNetwork();
    void updateMetrics();
    void liveTrackPoll();
    void onAIProcessFinished(int exitCode, QProcess::ExitStatus status);

private:
    void buildSidebar(QHBoxLayout* root);
    void buildMainContent(QHBoxLayout* root);
    QWidget* buildDashboardPage();
    QWidget* buildThreatFeedPage();
    QWidget* buildNetworkPage();
    QWidget* buildBlockchainPage();
    QWidget* buildAIReportPage();

    void processFile(const QString& filepath);
    std::vector<LogEvent> parseLogFile(const QString& filepath);
    void runDetectionRules(std::vector<LogEvent>& events);
    double calculateRiskScore(const std::vector<LogEvent>& events, QString& level);
    void updateDashboard();
    void updateFeedTable();
    void updateBlockchainView();

    // UI
    QStackedWidget* m_stack;
    QLabel* m_breadcrumb;
    QLabel* m_statusLabel;
    QVector<NavButton*> m_navButtons;

    // Sidebar metrics
    QLabel* m_cpuLabel;
    QProgressBar* m_cpuBar;
    QLabel* m_ramLabel;
    QProgressBar* m_ramBar;
    QLabel* m_diskLabel;
    QProgressBar* m_diskBar;

    // Dashboard
    RiskGauge* m_riskGauge;
    StatCard* m_cardEvents;
    StatCard* m_cardThreats;
    StatCard* m_cardBlocks;
    StatCard* m_cardConns;
    StatCard* m_cardRiskLevel;
    SeverityBar* m_sevBar;
    QTableWidget* m_recentTable;

    // Feed
    QTableWidget* m_feedTable;

    // Network
    QTableWidget* m_netTable;
    QLabel* m_netSummary;

    // Blockchain
    QTextEdit* m_bcText;

    // AI
    QTextEdit* m_aiText;
    QProcess* m_aiProcess;

    // Tracking
    QPushButton* m_btnTrack;
    QTimer* m_metricsTimer;
    QTimer* m_trackTimer;
    bool m_isTracking = false;
    QString m_liveFile;

    // Data
    std::vector<LogEvent> m_events;
    double m_riskScore = 0;
    QString m_riskLevel = "LOW";
    int m_blockCount = 1;
    QString m_pythonDir;
};

#endif // MAINWINDOW_H
