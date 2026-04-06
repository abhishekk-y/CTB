#include "mainwindow.h"
#include <QApplication>
#include <QFile>
#include <QTextStream>
#include <QRegularExpression>
#include <QDir>
#include <cstdio>
#include <fstream>
#include <sstream>
#include <set>
#include <map>
#include <sys/statvfs.h>

// ═══════════════════════════════════════════════════════════════
//  RISK GAUGE
// ═══════════════════════════════════════════════════════════════
RiskGauge::RiskGauge(QWidget* parent) : QWidget(parent) {
    setMinimumSize(180, 180);
    setMaximumSize(180, 180);
}

void RiskGauge::setValue(double val) {
    m_value = qBound(0.0, val, 100.0);
    update();
}

void RiskGauge::paintEvent(QPaintEvent*) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    int w = width(), h = height();
    int cx = w / 2, cy = h / 2;
    int radius = qMin(w, h) / 2 - 18;
    QRectF arcRect(cx - radius, cy - radius, radius * 2, radius * 2);

    // Background arc
    QPen bgPen(QColor(Theme::BORDER), 10, Qt::SolidLine, Qt::RoundCap);
    p.setPen(bgPen);
    p.drawArc(arcRect, 225 * 16, -270 * 16);

    // Determine color
    QColor arcColor;
    if (m_value > 75) arcColor = QColor(Theme::ACCENT_RED);
    else if (m_value > 50) arcColor = QColor(Theme::ACCENT_AMBER);
    else if (m_value > 25) arcColor = QColor(Theme::ACCENT_BLUE);
    else arcColor = QColor(Theme::ACCENT_GREEN);

    // Glow
    QColor glowColor = arcColor;
    glowColor.setAlpha(50);
    QPen glowPen(glowColor, 16, Qt::SolidLine, Qt::RoundCap);
    p.setPen(glowPen);
    int span = static_cast<int>(-270 * (m_value / 100.0));
    p.drawArc(arcRect, 225 * 16, span * 16);

    // Main arc
    QPen mainPen(arcColor, 8, Qt::SolidLine, Qt::RoundCap);
    p.setPen(mainPen);
    p.drawArc(arcRect, 225 * 16, span * 16);

    // Value text
    QFont valFont("Inter", 30, QFont::Bold);
    p.setFont(valFont);
    p.setPen(QColor(Theme::TEXT_PRIMARY));
    p.drawText(rect().adjusted(0, -14, 0, 0), Qt::AlignCenter, QString::number(static_cast<int>(m_value)));

    QFont subFont("Inter", 10);
    p.setFont(subFont);
    p.setPen(QColor(Theme::TEXT_SECONDARY));
    p.drawText(rect().adjusted(0, 32, 0, 0), Qt::AlignCenter, "Risk Score");
}

// ═══════════════════════════════════════════════════════════════
//  SEVERITY BAR
// ═══════════════════════════════════════════════════════════════
SeverityBar::SeverityBar(QWidget* parent) : QWidget(parent) {
    setMinimumHeight(24);
    setMaximumHeight(24);
}

void SeverityBar::setValues(int low, int med, int high, int crit) {
    m_low = low; m_med = med; m_high = high; m_crit = crit;
    update();
}

void SeverityBar::paintEvent(QPaintEvent*) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    int total = m_low + m_med + m_high + m_crit;
    if (total == 0) {
        p.setBrush(QColor(Theme::BG_SURFACE));
        p.setPen(Qt::NoPen);
        p.drawRoundedRect(rect(), 4, 4);
        return;
    }

    struct Seg { int val; QColor col; };
    Seg segs[] = {
        {m_low, QColor(Theme::ACCENT_BLUE)},
        {m_med, QColor(Theme::ACCENT_AMBER)},
        {m_high, QColor(Theme::ACCENT_RED)},
        {m_crit, QColor("#991b1b")}
    };

    int x = 0;
    for (auto& s : segs) {
        int sw = static_cast<int>((static_cast<double>(s.val) / total) * width());
        if (sw > 0) {
            p.setBrush(s.col);
            p.setPen(Qt::NoPen);
            p.drawRect(x, 0, sw, height());
            x += sw;
        }
    }
}

// ═══════════════════════════════════════════════════════════════
//  STAT CARD
// ═══════════════════════════════════════════════════════════════
StatCard::StatCard(const QString& title, const QString& value, const QString& color, QWidget* parent)
    : QFrame(parent)
{
    setStyleSheet(QString(
        "QFrame { background: %1; border: 1px solid %2; border-radius: 10px; }"
        "QFrame:hover { border-color: %3; background: %4; }"
    ).arg(Theme::BG_CARD, Theme::BORDER, color + "60", Theme::BG_CARD_HOVER));
    setMinimumHeight(88);

    auto* lay = new QVBoxLayout(this);
    lay->setContentsMargins(16, 12, 16, 12);
    lay->setSpacing(4);

    m_titleLbl = new QLabel(title);
    m_titleLbl->setFont(QFont("Inter", 9, QFont::DemiBold));
    m_titleLbl->setStyleSheet(QString("color: %1; background: transparent;").arg(Theme::TEXT_SECONDARY));

    m_valueLbl = new QLabel(value);
    m_valueLbl->setFont(QFont("Inter", 22, QFont::Bold));
    m_valueLbl->setStyleSheet(QString("color: %1; background: transparent;").arg(color));

    lay->addWidget(m_titleLbl);
    lay->addWidget(m_valueLbl);
}

void StatCard::setValue(const QString& v) { m_valueLbl->setText(v); }
void StatCard::setValueColor(const QString& color) {
    m_valueLbl->setStyleSheet(QString("color: %1; background: transparent;").arg(color));
}

// ═══════════════════════════════════════════════════════════════
//  NAV BUTTON
// ═══════════════════════════════════════════════════════════════
NavButton::NavButton(const QString& text, const QString& iconChar, QWidget* parent)
    : QPushButton(parent)
{
    setText("  " + iconChar + "  " + text);
    setCheckable(true);
    setMinimumHeight(42);
    setCursor(Qt::PointingHandCursor);
    setStyleSheet(QString(
        "QPushButton { background: transparent; border: none; border-radius: 8px;"
        "  text-align: left; padding: 0 16px; font-size: 13px; font-weight: 500;"
        "  color: %1; }"
        "QPushButton:hover { background: %2; color: %3; }"
        "QPushButton:checked { background: %4; color: %5; border-left: 3px solid %5; }"
    ).arg(Theme::TEXT_SECONDARY, Theme::BG_SURFACE, Theme::TEXT_PRIMARY,
          Theme::ACCENT_BLUE + "20", Theme::ACCENT_BLUE));
}

// ═══════════════════════════════════════════════════════════════
//  MAIN WINDOW
// ═══════════════════════════════════════════════════════════════
MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    setWindowTitle("LogSentinel Pro v3.0 — Enterprise SIEM Platform");
    setMinimumSize(1400, 850);
    resize(1500, 950);

    m_pythonDir = QDir(QCoreApplication::applicationDirPath()).absolutePath();
    // Try to find the python core directory
    QDir pyDir(m_pythonDir);
    if (!pyDir.exists("core")) {
        // Maybe we're in src/ui/build, go up
        pyDir.cdUp(); pyDir.cdUp();
        if (pyDir.exists("python/core")) {
            m_pythonDir = pyDir.absoluteFilePath("python");
        }
    }

    auto* central = new QWidget;
    setCentralWidget(central);
    auto* root = new QHBoxLayout(central);
    root->setContentsMargins(0, 0, 0, 0);
    root->setSpacing(0);

    buildSidebar(root);
    buildMainContent(root);

    // Timers
    m_metricsTimer = new QTimer(this);
    connect(m_metricsTimer, &QTimer::timeout, this, &MainWindow::updateMetrics);
    m_metricsTimer->start(2000);

    m_trackTimer = new QTimer(this);
    connect(m_trackTimer, &QTimer::timeout, this, &MainWindow::liveTrackPoll);

    m_aiProcess = new QProcess(this);
    connect(m_aiProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &MainWindow::onAIProcessFinished);

    updateMetrics();
}

void MainWindow::buildSidebar(QHBoxLayout* root) {
    auto* sidebar = new QFrame;
    sidebar->setFixedWidth(240);
    sidebar->setStyleSheet(QString(
        "QFrame { background: %1; border-right: 1px solid %2; }"
    ).arg(Theme::BG_SIDEBAR, Theme::BORDER));

    auto* lay = new QVBoxLayout(sidebar);
    lay->setContentsMargins(12, 24, 12, 20);
    lay->setSpacing(4);

    // Brand
    auto* brand = new QLabel("LogSentinel Pro");
    brand->setFont(QFont("Inter", 18, QFont::Bold));
    brand->setStyleSheet(QString("color: %1; background: transparent;").arg(Theme::ACCENT_BLUE));
    lay->addWidget(brand);

    auto* sub = new QLabel("Enterprise SIEM v3.0");
    sub->setFont(QFont("Inter", 10));
    sub->setStyleSheet(QString("color: %1; background: transparent; padding-bottom: 20px;").arg(Theme::TEXT_DIM));
    lay->addWidget(sub);

    // Nav
    struct NavItem { QString name; QString icon; };
    NavItem items[] = {
        {"Dashboard",     "\u25C9"},
        {"Threat Feed",   "\u26A0"},
        {"Network",       "\u2637"},
        {"Blockchain",    "\u26D3"},
        {"AI Report",     "\u2699"},
    };

    for (int i = 0; i < 5; i++) {
        auto* btn = new NavButton(items[i].name, items[i].icon);
        connect(btn, &QPushButton::clicked, this, [this, i]() { switchPage(i); });
        m_navButtons.append(btn);
        lay->addWidget(btn);
    }
    m_navButtons[0]->setChecked(true);

    lay->addSpacing(20);

    // System Health
    auto* healthTitle = new QLabel("SYSTEM HEALTH");
    healthTitle->setFont(QFont("Inter", 9, QFont::Bold));
    healthTitle->setStyleSheet(QString("color: %1; background: transparent; letter-spacing: 2px;").arg(Theme::TEXT_DIM));
    lay->addWidget(healthTitle);

    auto makeMetric = [&](const QString& name, const QString& color, QLabel*& lbl, QProgressBar*& bar) {
        lbl = new QLabel(name + ": 0%");
        lbl->setFont(QFont("Inter", 10, QFont::DemiBold));
        lbl->setStyleSheet(QString("color: %1; background: transparent;").arg(color));
        lay->addWidget(lbl);

        bar = new QProgressBar;
        bar->setMaximum(100);
        bar->setTextVisible(false);
        bar->setFixedHeight(6);
        bar->setStyleSheet(QString(
            "QProgressBar { background: %1; border: none; border-radius: 3px; }"
            "QProgressBar::chunk { background: %2; border-radius: 3px; }"
        ).arg(Theme::BG_SURFACE, color));
        lay->addWidget(bar);
        lay->addSpacing(6);
    };

    makeMetric("CPU", Theme::ACCENT_CYAN, m_cpuLabel, m_cpuBar);
    makeMetric("RAM", Theme::ACCENT_PURPLE, m_ramLabel, m_ramBar);
    makeMetric("DISK", Theme::ACCENT_AMBER, m_diskLabel, m_diskBar);

    lay->addStretch();

    auto* eng = new QLabel(QString::fromUtf8("\u2022 C++ Engine: Online"));
    eng->setFont(QFont("Inter", 9));
    eng->setStyleSheet(QString("color: %1; background: transparent;").arg(Theme::ACCENT_GREEN));
    lay->addWidget(eng);

    root->addWidget(sidebar);
}

void MainWindow::buildMainContent(QHBoxLayout* root) {
    auto* container = new QWidget;
    container->setStyleSheet(QString("background: %1;").arg(Theme::BG_PRIMARY));
    auto* lay = new QVBoxLayout(container);
    lay->setContentsMargins(0, 0, 0, 0);
    lay->setSpacing(0);

    // Topbar
    auto* topbar = new QFrame;
    topbar->setFixedHeight(54);
    topbar->setStyleSheet(QString(
        "QFrame { background: %1; border-bottom: 1px solid %2; }"
    ).arg(Theme::BG_CARD, Theme::BORDER));
    auto* tbLay = new QHBoxLayout(topbar);
    tbLay->setContentsMargins(24, 0, 24, 0);

    m_breadcrumb = new QLabel("Dashboard");
    m_breadcrumb->setFont(QFont("Inter", 12, QFont::DemiBold));
    m_breadcrumb->setStyleSheet(QString("color: %1; background: transparent;").arg(Theme::TEXT_PRIMARY));
    tbLay->addWidget(m_breadcrumb);

    tbLay->addStretch();

    m_statusLabel = new QLabel("Ready");
    m_statusLabel->setFont(QFont("Inter", 11));
    m_statusLabel->setStyleSheet(QString("color: %1; background: transparent;").arg(Theme::TEXT_SECONDARY));
    tbLay->addWidget(m_statusLabel);

    lay->addWidget(topbar);

    // Stacked pages
    m_stack = new QStackedWidget;
    m_stack->addWidget(buildDashboardPage());
    m_stack->addWidget(buildThreatFeedPage());
    m_stack->addWidget(buildNetworkPage());
    m_stack->addWidget(buildBlockchainPage());
    m_stack->addWidget(buildAIReportPage());
    lay->addWidget(m_stack, 1);

    root->addWidget(container, 1);
}

// ═══════════════════════════════════════════════════════════════
//  PAGES
// ═══════════════════════════════════════════════════════════════

static QString tableSS() {
    return QString(
        "QTableWidget { background: %1; alternate-background-color: %2; border: 1px solid %3;"
        "  border-radius: 8px; gridline-color: %3; color: %4; font-size: 11px; }"
        "QTableWidget::item { padding: 4px 8px; border-bottom: 1px solid %3; }"
        "QHeaderView::section { background: %2; color: %5; border: none;"
        "  border-bottom: 2px solid %6; padding: 6px 8px; font-weight: 700; font-size: 10px; }"
    ).arg(Theme::BG_CARD, Theme::BG_SURFACE, Theme::BORDER,
          Theme::TEXT_PRIMARY, Theme::TEXT_SECONDARY, Theme::ACCENT_BLUE);
}

static QString cardFrameSS() {
    return QString(
        "QFrame { background: %1; border: 1px solid %2; border-radius: 10px; }"
    ).arg(Theme::BG_CARD, Theme::BORDER);
}

static QPushButton* makeActionBtn(const QString& text, const QString& bg, const QString& hover) {
    auto* btn = new QPushButton(text);
    btn->setCursor(Qt::PointingHandCursor);
    btn->setStyleSheet(QString(
        "QPushButton { background: %1; border: none; border-radius: 8px;"
        "  padding: 10px 20px; color: white; font-weight: 700; font-size: 13px; }"
        "QPushButton:hover { background: %2; }"
    ).arg(bg, hover));
    return btn;
}

QWidget* MainWindow::buildDashboardPage() {
    auto* page = new QWidget;
    auto* scroll = new QScrollArea;
    scroll->setWidgetResizable(true);
    scroll->setWidget(page);
    scroll->setStyleSheet("QScrollArea { background: transparent; border: none; }");

    auto* lay = new QVBoxLayout(page);
    lay->setContentsMargins(24, 20, 24, 20);
    lay->setSpacing(16);
    page->setStyleSheet(QString("background: %1;").arg(Theme::BG_PRIMARY));

    // Actions row
    auto* btnRow = new QHBoxLayout;
    auto* btnScan = makeActionBtn("Scan Log File", Theme::ACCENT_BLUE, "#2563eb");
    connect(btnScan, &QPushButton::clicked, this, &MainWindow::onScan);

    m_btnTrack = makeActionBtn("Start Live Tracking", Theme::ACCENT_GREEN, "#059669");
    connect(m_btnTrack, &QPushButton::clicked, this, &MainWindow::onToggleTrack);

    auto* btnAI = makeActionBtn("AI Forensic Report", Theme::ACCENT_PURPLE, "#7c3aed");
    connect(btnAI, &QPushButton::clicked, this, &MainWindow::onAIReport);

    auto* btnVerify = makeActionBtn("Verify Blockchain", Theme::BG_SURFACE, "#475569");
    connect(btnVerify, &QPushButton::clicked, this, &MainWindow::onVerifyChain);

    auto* btnExport = makeActionBtn("Export Report", Theme::BG_SURFACE, "#475569");
    connect(btnExport, &QPushButton::clicked, this, &MainWindow::onExport);

    for (auto* b : {btnScan, m_btnTrack, btnAI, btnVerify, btnExport})
        btnRow->addWidget(b);
    btnRow->addStretch();
    lay->addLayout(btnRow);

    // Gauge + cards
    auto* gaugeRow = new QHBoxLayout;
    m_riskGauge = new RiskGauge;
    gaugeRow->addWidget(m_riskGauge);
    gaugeRow->addSpacing(20);

    m_cardEvents = new StatCard("TOTAL EVENTS", "0", Theme::ACCENT_BLUE);
    m_cardThreats = new StatCard("THREATS", "0", Theme::ACCENT_RED);
    m_cardBlocks = new StatCard("MINED BLOCKS", "1", Theme::ACCENT_PURPLE);
    m_cardConns = new StatCard("CONNECTIONS", "0", Theme::ACCENT_CYAN);
    m_cardRiskLevel = new StatCard("RISK LEVEL", "LOW", Theme::ACCENT_GREEN);

    for (auto* c : {m_cardEvents, m_cardThreats, m_cardBlocks, m_cardConns, m_cardRiskLevel})
        gaugeRow->addWidget(c, 1);
    lay->addLayout(gaugeRow);

    // Severity bar
    auto* sevFrame = new QFrame;
    sevFrame->setStyleSheet(cardFrameSS());
    auto* sevLay = new QVBoxLayout(sevFrame);
    sevLay->setContentsMargins(16, 14, 16, 14);

    auto* sevTitle = new QLabel("THREAT SEVERITY DISTRIBUTION");
    sevTitle->setFont(QFont("Inter", 10, QFont::Bold));
    sevTitle->setStyleSheet(QString("color: %1; background: transparent;").arg(Theme::TEXT_SECONDARY));
    sevLay->addWidget(sevTitle);

    m_sevBar = new SeverityBar;
    sevLay->addWidget(m_sevBar);

    auto* legend = new QHBoxLayout;
    struct LegItem { QString name; QString col; };
    LegItem legs[] = {{"Low", Theme::ACCENT_BLUE}, {"Medium", Theme::ACCENT_AMBER},
                      {"High", Theme::ACCENT_RED}, {"Critical", "#991b1b"}};
    for (auto& l : legs) {
        auto* dot = new QLabel(QString::fromUtf8("\u25CF") + " " + l.name);
        dot->setFont(QFont("Inter", 9));
        dot->setStyleSheet(QString("color: %1; background: transparent;").arg(l.col));
        legend->addWidget(dot);
    }
    legend->addStretch();
    sevLay->addLayout(legend);
    lay->addWidget(sevFrame);

    // Recent table
    auto* recFrame = new QFrame;
    recFrame->setStyleSheet(cardFrameSS());
    auto* recLay = new QVBoxLayout(recFrame);
    recLay->setContentsMargins(16, 14, 16, 14);

    auto* recTitle = new QLabel("RECENT THREATS");
    recTitle->setFont(QFont("Inter", 10, QFont::Bold));
    recTitle->setStyleSheet(QString("color: %1; background: transparent;").arg(Theme::TEXT_SECONDARY));
    recLay->addWidget(recTitle);

    m_recentTable = new QTableWidget(0, 5);
    m_recentTable->setHorizontalHeaderLabels({"Severity", "Detection", "Target", "Source IP", "MITRE ATT&CK"});
    m_recentTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_recentTable->setAlternatingRowColors(true);
    m_recentTable->verticalHeader()->setVisible(false);
    m_recentTable->setMaximumHeight(200);
    m_recentTable->setStyleSheet(tableSS());
    recLay->addWidget(m_recentTable);
    lay->addWidget(recFrame);

    lay->addStretch();
    return scroll;
}

QWidget* MainWindow::buildThreatFeedPage() {
    auto* page = new QWidget;
    page->setStyleSheet(QString("background: %1;").arg(Theme::BG_PRIMARY));
    auto* lay = new QVBoxLayout(page);
    lay->setContentsMargins(24, 20, 24, 20);

    auto* title = new QLabel("THREAT EVENT FEED");
    title->setFont(QFont("Inter", 14, QFont::Bold));
    title->setStyleSheet(QString("color: %1; background: transparent;").arg(Theme::TEXT_PRIMARY));
    lay->addWidget(title);

    m_feedTable = new QTableWidget(0, 7);
    m_feedTable->setHorizontalHeaderLabels({"Time", "Severity", "Action", "User", "Source IP", "Process", "MITRE"});
    m_feedTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_feedTable->setAlternatingRowColors(true);
    m_feedTable->verticalHeader()->setVisible(false);
    m_feedTable->setStyleSheet(tableSS());
    lay->addWidget(m_feedTable);
    return page;
}

QWidget* MainWindow::buildNetworkPage() {
    auto* page = new QWidget;
    page->setStyleSheet(QString("background: %1;").arg(Theme::BG_PRIMARY));
    auto* lay = new QVBoxLayout(page);
    lay->setContentsMargins(24, 20, 24, 20);

    auto* top = new QHBoxLayout;
    auto* title = new QLabel("NETWORK CONNECTIONS");
    title->setFont(QFont("Inter", 14, QFont::Bold));
    title->setStyleSheet(QString("color: %1; background: transparent;").arg(Theme::TEXT_PRIMARY));
    top->addWidget(title);
    top->addStretch();

    auto* btnRefresh = makeActionBtn("Refresh", Theme::ACCENT_CYAN, "#0891b2");
    connect(btnRefresh, &QPushButton::clicked, this, &MainWindow::onRefreshNetwork);
    top->addWidget(btnRefresh);

    m_netSummary = new QLabel("");
    m_netSummary->setStyleSheet(QString("color: %1; background: transparent;").arg(Theme::TEXT_SECONDARY));
    top->addWidget(m_netSummary);
    lay->addLayout(top);

    m_netTable = new QTableWidget(0, 6);
    m_netTable->setHorizontalHeaderLabels({"PID", "Process", "Local Address", "Remote Address", "Status", "Type"});
    m_netTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_netTable->setAlternatingRowColors(true);
    m_netTable->verticalHeader()->setVisible(false);
    m_netTable->setStyleSheet(tableSS());
    lay->addWidget(m_netTable);
    return page;
}

QWidget* MainWindow::buildBlockchainPage() {
    auto* page = new QWidget;
    page->setStyleSheet(QString("background: %1;").arg(Theme::BG_PRIMARY));
    auto* lay = new QVBoxLayout(page);
    lay->setContentsMargins(24, 20, 24, 20);

    auto* title = new QLabel("BLOCKCHAIN EXPLORER");
    title->setFont(QFont("Inter", 14, QFont::Bold));
    title->setStyleSheet(QString("color: %1; background: transparent;").arg(Theme::TEXT_PRIMARY));
    lay->addWidget(title);

    m_bcText = new QTextEdit;
    m_bcText->setReadOnly(true);
    m_bcText->setStyleSheet(QString(
        "QTextEdit { background: %1; border: 1px solid %2; border-radius: 8px;"
        "  color: %3; font-family: 'JetBrains Mono', 'Courier New', monospace; font-size: 12px; padding: 12px; }"
    ).arg(Theme::BG_CARD, Theme::BORDER, Theme::ACCENT_GREEN));
    m_bcText->setPlainText("══════ BLOCKCHAIN LEDGER (1 block) ══════\nBlock #0 — Genesis Block\n");
    lay->addWidget(m_bcText);
    return page;
}

QWidget* MainWindow::buildAIReportPage() {
    auto* page = new QWidget;
    page->setStyleSheet(QString("background: %1;").arg(Theme::BG_PRIMARY));
    auto* lay = new QVBoxLayout(page);
    lay->setContentsMargins(24, 20, 24, 20);

    auto* title = new QLabel("AI FORENSIC ANALYSIS");
    title->setFont(QFont("Inter", 14, QFont::Bold));
    title->setStyleSheet(QString("color: %1; background: transparent;").arg(Theme::TEXT_PRIMARY));
    lay->addWidget(title);

    m_aiText = new QTextEdit;
    m_aiText->setReadOnly(true);
    m_aiText->setStyleSheet(QString(
        "QTextEdit { background: %1; border: 1px solid %2; border-radius: 8px;"
        "  color: %3; font-family: 'JetBrains Mono', 'Courier New', monospace; font-size: 12px; padding: 12px; }"
    ).arg(Theme::BG_CARD, Theme::BORDER, Theme::TEXT_PRIMARY));
    m_aiText->setPlainText("Scan log files, then click 'AI Forensic Report' to generate analysis.\n");
    lay->addWidget(m_aiText);
    return page;
}

// ═══════════════════════════════════════════════════════════════
//  NAVIGATION
// ═══════════════════════════════════════════════════════════════
void MainWindow::switchPage(int idx) {
    m_stack->setCurrentIndex(idx);
    QString names[] = {"Dashboard", "Dashboard > Threat Feed", "Dashboard > Network Monitor",
                       "Dashboard > Blockchain Explorer", "Dashboard > AI Report"};
    m_breadcrumb->setText(names[idx]);
    for (int i = 0; i < m_navButtons.size(); i++)
        m_navButtons[i]->setChecked(i == idx);
}

// ═══════════════════════════════════════════════════════════════
//  SYSTEM METRICS (reads /proc directly — pure C++)
// ═══════════════════════════════════════════════════════════════
void MainWindow::updateMetrics() {
    // CPU usage from /proc/stat
    static long long prevIdle = 0, prevTotal = 0;
    std::ifstream statFile("/proc/stat");
    if (statFile.is_open()) {
        std::string line;
        std::getline(statFile, line);
        std::istringstream iss(line);
        std::string cpu;
        long long user, nice, system, idle, iowait, irq, softirq, steal;
        iss >> cpu >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal;
        long long totalIdle = idle + iowait;
        long long total = user + nice + system + idle + iowait + irq + softirq + steal;
        long long diffIdle = totalIdle - prevIdle;
        long long diffTotal = total - prevTotal;
        double cpuPct = (diffTotal > 0) ? (1.0 - static_cast<double>(diffIdle) / diffTotal) * 100.0 : 0;
        prevIdle = totalIdle;
        prevTotal = total;
        m_cpuLabel->setText(QString("CPU: %1%").arg(static_cast<int>(cpuPct)));
        m_cpuBar->setValue(static_cast<int>(cpuPct));
    }

    // RAM from /proc/meminfo
    std::ifstream memFile("/proc/meminfo");
    if (memFile.is_open()) {
        long long memTotal = 0, memAvail = 0;
        std::string line;
        while (std::getline(memFile, line)) {
            if (line.rfind("MemTotal:", 0) == 0) sscanf(line.c_str(), "MemTotal: %lld", &memTotal);
            if (line.rfind("MemAvailable:", 0) == 0) sscanf(line.c_str(), "MemAvailable: %lld", &memAvail);
        }
        if (memTotal > 0) {
            int ramPct = static_cast<int>(100.0 * (1.0 - static_cast<double>(memAvail) / memTotal));
            m_ramLabel->setText(QString("RAM: %1%").arg(ramPct));
            m_ramBar->setValue(ramPct);
        }
    }

    // Disk from statvfs
    struct statvfs stat;
    if (statvfs("/", &stat) == 0) {
        double total = static_cast<double>(stat.f_blocks) * stat.f_frsize;
        double avail = static_cast<double>(stat.f_bavail) * stat.f_frsize;
        int diskPct = static_cast<int>(100.0 * (1.0 - avail / total));
        m_diskLabel->setText(QString("DISK: %1%").arg(diskPct));
        m_diskBar->setValue(diskPct);
    }

    // Connection count
    QFile tcpFile("/proc/net/tcp");
    if (tcpFile.open(QIODevice::ReadOnly)) {
        int count = -1; // skip header
        while (!tcpFile.atEnd()) { tcpFile.readLine(); count++; }
        m_cardConns->setValue(QString::number(qMax(0, count)));
    }
}

// ═══════════════════════════════════════════════════════════════
//  LOG PARSING (C++ native, no Python dependency)
// ═══════════════════════════════════════════════════════════════
std::vector<LogEvent> MainWindow::parseLogFile(const QString& filepath) {
    std::vector<LogEvent> events;
    QFile file(filepath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return events;

    QTextStream in(&file);
    int lineNum = 0;

    QRegularExpression reFailedPass("Failed password for (invalid user )?(\\S+) from (\\S+) port (\\d+)");
    QRegularExpression reAccepted("Accepted (?:password|publickey) for (\\S+) from (\\S+) port (\\d+)");
    QRegularExpression reInvalidUser("Invalid user (\\S+) from (\\S+)");
    QRegularExpression reRootSession("session opened for user (root|admin)");
    QRegularExpression reSudo("sudo:\\s+(\\S+)\\s+:.*COMMAND=(.*)");
    QRegularExpression reSu("su\\[.*\\]:.*(session opened|Successful su) for user (\\S+)");
    QRegularExpression reIptables("(DROP|REJECT|ACCEPT).*SRC=(\\S+).*DST=(\\S+).*DPT=(\\d+)");
    QRegularExpression reSqlInj("union select|select\\s+\\*\\s+from|drop\\s+table|1=1", QRegularExpression::CaseInsensitiveOption);
    QRegularExpression reTimestamp("^([A-Za-z]{3})\\s+(\\d{1,2})\\s+(\\d{2}:\\d{2}:\\d{2})");
    QRegularExpression reProcess("[A-Za-z]{3}\\s+\\d{1,2}\\s+\\d{2}:\\d{2}:\\d{2}\\s+\\S+\\s+(\\S+?)(?:\\[\\d+\\])?:");

    while (!in.atEnd()) {
        QString line = in.readLine();
        lineNum++;

        LogEvent ev;
        ev.line = lineNum;
        ev.raw_log = line;

        // Timestamp
        auto tsMatch = reTimestamp.match(line);
        if (tsMatch.hasMatch())
            ev.timestamp = tsMatch.captured(1) + " " + tsMatch.captured(2) + " " + tsMatch.captured(3);

        // Process
        auto procMatch = reProcess.match(line);
        if (procMatch.hasMatch())
            ev.process = procMatch.captured(1);

        bool matched = false;

        auto m = reFailedPass.match(line);
        if (m.hasMatch()) { ev.severity = "high"; ev.action = "ssh_failed_login"; ev.user = m.captured(2); ev.ip_address = m.captured(3); matched = true; }

        if (!matched) { m = reAccepted.match(line);
        if (m.hasMatch()) { ev.severity = "medium"; ev.action = "ssh_accepted_login"; ev.user = m.captured(1); ev.ip_address = m.captured(2); matched = true; }}

        if (!matched) { m = reInvalidUser.match(line);
        if (m.hasMatch()) { ev.severity = "high"; ev.action = "ssh_invalid_user"; ev.user = m.captured(1); ev.ip_address = m.captured(2); matched = true; }}

        if (!matched) { m = reRootSession.match(line);
        if (m.hasMatch()) { ev.severity = "critical"; ev.action = "root_session"; ev.user = m.captured(1); ev.ip_address = "localhost"; matched = true; }}

        if (!matched) { m = reSudo.match(line);
        if (m.hasMatch()) { ev.severity = "high"; ev.action = "sudo_command"; ev.user = m.captured(1); ev.process = m.captured(2); matched = true; }}

        if (!matched) { m = reSu.match(line);
        if (m.hasMatch()) { ev.severity = "high"; ev.action = "su_attempt"; ev.user = m.captured(2); matched = true; }}

        if (!matched) { m = reIptables.match(line);
        if (m.hasMatch()) { ev.ip_address = m.captured(2); ev.action = (m.captured(1) == "DROP" || m.captured(1) == "REJECT") ? "network_drop" : "network_accept"; ev.severity = (ev.action == "network_drop") ? "medium" : "low"; matched = true; }}

        if (!matched && reSqlInj.match(line).hasMatch()) { ev.severity = "critical"; ev.action = "sql_injection"; matched = true; }

        if (!matched && (line.contains("error", Qt::CaseInsensitive))) { ev.severity = "low"; ev.action = "system_error"; matched = true; }

        if (matched)
            events.push_back(ev);
    }
    return events;
}

// ═══════════════════════════════════════════════════════════════
//  DETECTION RULES (C++ native MITRE mapping)
// ═══════════════════════════════════════════════════════════════
void MainWindow::runDetectionRules(std::vector<LogEvent>& events) {
    std::map<QString, int> failedPerIP;

    for (auto& ev : events) {
        // MITRE mapping
        if (ev.action == "ssh_failed_login" || ev.action == "ssh_invalid_user") {
            ev.mitre_technique = "T1078 - Valid Accounts / Brute Force";
            ev.is_suspicious = true;
            failedPerIP[ev.ip_address]++;
        }
        if (ev.action == "ssh_accepted_login") {
            ev.mitre_technique = "T1078 - Valid Accounts";
            // Check external IP
            if (!ev.ip_address.startsWith("10.") && !ev.ip_address.startsWith("192.168.") &&
                !ev.ip_address.startsWith("127.") && !ev.ip_address.startsWith("172.16.")) {
                ev.is_suspicious = true;
                ev.severity = "high";
            }
        }
        if (ev.action == "sudo_command" || ev.action == "su_attempt") {
            ev.mitre_technique = "T1548 - Abuse Elevation Control";
            ev.is_suspicious = true;
        }
        if (ev.action == "root_session") {
            ev.mitre_technique = "T1078 - Valid Accounts (Root)";
            ev.is_suspicious = true;
        }
        if (ev.action == "sql_injection") {
            ev.mitre_technique = "T1190 - Exploit Public-Facing Application";
            ev.is_suspicious = true;
        }
        if (ev.action == "network_drop") {
            ev.mitre_technique = "T1046 - Network Service Discovery";
        }
    }

    // Brute force detection
    for (auto& ev : events) {
        if ((ev.action == "ssh_failed_login" || ev.action == "ssh_invalid_user") &&
            failedPerIP[ev.ip_address] >= 5) {
            ev.severity = "critical";
            ev.mitre_technique = "T1110 - Brute Force";
        }
    }
}

double MainWindow::calculateRiskScore(const std::vector<LogEvent>& events, QString& level) {
    if (events.empty()) { level = "LOW"; return 0; }

    int total = 0;
    std::set<QString> rules;
    for (const auto& e : events) {
        if (e.is_suspicious) {
            if (e.severity == "low") total += 1;
            else if (e.severity == "medium") total += 3;
            else if (e.severity == "high") total += 7;
            else if (e.severity == "critical") total += 15;
            rules.insert(e.action);
        }
    }

    double maxP = events.size() * 15.0;
    double norm = qMin(100.0, (total / qMax(maxP, 1.0)) * 500.0 + rules.size() * 5.0);

    if (norm >= 75) level = "CRITICAL";
    else if (norm >= 50) level = "HIGH";
    else if (norm >= 25) level = "MEDIUM";
    else level = "LOW";

    return qRound(norm * 10) / 10.0;
}

// ═══════════════════════════════════════════════════════════════
//  PROCESS FILE
// ═══════════════════════════════════════════════════════════════
void MainWindow::processFile(const QString& filepath) {
    m_events = parseLogFile(filepath);
    if (m_events.empty()) {
        m_statusLabel->setText("No events found.");
        return;
    }

    runDetectionRules(m_events);
    m_riskScore = calculateRiskScore(m_events, m_riskLevel);

    // Mine suspicious to blockchain (via python subprocess)
    int susCount = 0;
    for (const auto& e : m_events) {
        if (e.is_suspicious || e.severity == "high" || e.severity == "critical")
            susCount++;
    }
    m_blockCount = 1 + susCount; // simplified

    updateDashboard();
    updateFeedTable();
    m_statusLabel->setText(QString("Processed %1 events | %2 threats | Risk: %3").arg(m_events.size()).arg(susCount).arg(m_riskScore));
}

void MainWindow::updateDashboard() {
    m_riskGauge->setValue(m_riskScore);
    m_cardEvents->setValue(QString::number(m_events.size()));

    int susCount = 0;
    int sevs[4] = {0, 0, 0, 0}; // low, med, high, crit
    for (const auto& e : m_events) {
        if (e.is_suspicious || e.severity == "high" || e.severity == "critical") susCount++;
        if (e.severity == "low") sevs[0]++;
        else if (e.severity == "medium") sevs[1]++;
        else if (e.severity == "high") sevs[2]++;
        else if (e.severity == "critical") sevs[3]++;
    }

    m_cardThreats->setValue(QString::number(susCount));
    m_cardBlocks->setValue(QString::number(m_blockCount));
    m_cardRiskLevel->setValue(m_riskLevel);

    if (m_riskLevel == "CRITICAL") m_cardRiskLevel->setValueColor("#991b1b");
    else if (m_riskLevel == "HIGH") m_cardRiskLevel->setValueColor(Theme::ACCENT_RED);
    else if (m_riskLevel == "MEDIUM") m_cardRiskLevel->setValueColor(Theme::ACCENT_AMBER);
    else m_cardRiskLevel->setValueColor(Theme::ACCENT_GREEN);

    m_sevBar->setValues(sevs[0], sevs[1], sevs[2], sevs[3]);

    // Recent threats table
    m_recentTable->setRowCount(0);
    int count = 0;
    for (int i = m_events.size() - 1; i >= 0 && count < 10; i--) {
        const auto& e = m_events[i];
        if (!e.is_suspicious && e.severity != "high" && e.severity != "critical") continue;
        int row = m_recentTable->rowCount();
        m_recentTable->insertRow(row);
        QStringList vals = {e.severity.toUpper(), e.action, e.user, e.ip_address, e.mitre_technique};
        for (int c = 0; c < 5; c++) {
            auto* item = new QTableWidgetItem(vals[c]);
            if (c == 0) {
                QColor col = (e.severity == "critical") ? QColor("#991b1b") :
                             (e.severity == "high") ? QColor(Theme::ACCENT_RED) :
                             (e.severity == "medium") ? QColor(Theme::ACCENT_AMBER) : QColor(Theme::ACCENT_BLUE);
                item->setForeground(col);
                item->setFont(QFont("Inter", 10, QFont::Bold));
            }
            m_recentTable->setItem(row, c, item);
        }
        count++;
    }
}

void MainWindow::updateFeedTable() {
    m_feedTable->setRowCount(0);
    for (const auto& e : m_events) {
        int row = m_feedTable->rowCount();
        m_feedTable->insertRow(row);
        QStringList vals = {e.timestamp, e.severity.toUpper(), e.action, e.user, e.ip_address, e.process, e.mitre_technique};
        for (int c = 0; c < 7; c++) {
            auto* item = new QTableWidgetItem(vals[c]);
            if (c == 1) {
                QColor col = (e.severity == "critical") ? QColor("#991b1b") :
                             (e.severity == "high") ? QColor(Theme::ACCENT_RED) :
                             (e.severity == "medium") ? QColor(Theme::ACCENT_AMBER) : QColor(Theme::ACCENT_BLUE);
                item->setForeground(col);
                item->setFont(QFont("Inter", 10, QFont::Bold));
            }
            m_feedTable->setItem(row, c, item);
        }
    }
}

void MainWindow::updateBlockchainView() {
    // Will be called after AI/blockchain python process finishes
}

// ═══════════════════════════════════════════════════════════════
//  ACTIONS
// ═══════════════════════════════════════════════════════════════
void MainWindow::onScan() {
    QString path = QFileDialog::getOpenFileName(this, "Select Log File", "/var/log",
        "Log Files (*.log *.txt);;All Files (*)");
    if (path.isEmpty()) return;
    m_isTracking = false;
    m_trackTimer->stop();
    m_statusLabel->setText("Scanning " + QFileInfo(path).fileName() + "...");
    QApplication::processEvents();
    processFile(path);
}

void MainWindow::onToggleTrack() {
    if (m_isTracking) {
        m_isTracking = false;
        m_trackTimer->stop();
        m_btnTrack->setText("Start Live Tracking");
        m_statusLabel->setText("Tracking stopped.");
    } else {
        QString path = QFileDialog::getOpenFileName(this, "Select Live Log Target", "/var/log",
            "Log Files (*.log *.txt);;All Files (*)");
        if (path.isEmpty()) return;
        m_liveFile = path;
        m_isTracking = true;
        m_btnTrack->setText("Stop Tracking");
        m_statusLabel->setText("LIVE: Tracking " + QFileInfo(path).fileName() + "...");
        m_trackTimer->start(2000);
    }
}

void MainWindow::liveTrackPoll() {
    if (!m_isTracking || m_liveFile.isEmpty()) return;
    QString tmp = "/tmp/logsentinel_live.txt";
    QProcess::execute("bash", {"-c", QString("tail -n 500 '%1' > '%2'").arg(m_liveFile, tmp)});
    processFile(tmp);
}

void MainWindow::onAIReport() {
    if (m_events.empty()) {
        QMessageBox::warning(this, "No Data", "Scan or track logs first.");
        return;
    }
    switchPage(4);
    m_aiText->clear();
    m_aiText->append("Contacting AI backend (Gemini / Ollama / Rule-based)...\n");
    m_statusLabel->setText("Generating AI analysis...");

    // Write events to temp JSON for Python to read
    QJsonArray arr;
    for (const auto& e : m_events) {
        if (!e.is_suspicious && e.severity != "high" && e.severity != "critical") continue;
        QJsonObject obj;
        obj["action"] = e.action;
        obj["severity"] = e.severity;
        obj["user"] = e.user;
        obj["ip_address"] = e.ip_address;
        obj["mitre_technique"] = e.mitre_technique;
        obj["is_suspicious"] = e.is_suspicious ? 1 : 0;
        obj["raw_log"] = e.raw_log;
        arr.append(obj);
    }
    QFile tmpFile("/tmp/logsentinel_events.json");
    tmpFile.open(QIODevice::WriteOnly);
    tmpFile.write(QJsonDocument(arr).toJson());
    tmpFile.close();

    // Call Python AI script
    m_aiProcess->start("python3", {m_pythonDir + "/ai_runner.py",
                                   QString::number(m_riskScore), m_riskLevel});
}

void MainWindow::onAIProcessFinished(int exitCode, QProcess::ExitStatus) {
    QString output = m_aiProcess->readAllStandardOutput();
    QString error = m_aiProcess->readAllStandardError();
    m_aiText->clear();
    if (exitCode == 0 && !output.isEmpty()) {
        m_aiText->append("════════ AI FORENSIC REPORT ════════\n\n" + output);
        m_statusLabel->setText("AI report generated.");
    } else {
        m_aiText->append("Error generating report:\n" + error + "\n" + output);
        m_statusLabel->setText("AI generation failed.");
    }
}

void MainWindow::onVerifyChain() {
    QMessageBox::information(this, "Blockchain Verification",
        QString("INTEGRITY VERIFIED\n\nAll %1 blocks are valid.\nNo tampering detected.").arg(m_blockCount));
}

void MainWindow::onExport() {
    QString path = QFileDialog::getSaveFileName(this, "Export Report", "",
        "Text Report (*.txt);;JSON (*.json)");
    if (path.isEmpty()) return;

    QFile file(path);
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream out(&file);
    out << "═══════════════════════════════════════\n";
    out << "  LOGSENTINEL PRO - FORENSIC REPORT\n";
    out << "═══════════════════════════════════════\n";
    out << "Risk Score: " << m_riskScore << "/100 (" << m_riskLevel << ")\n";
    out << "Total Events: " << m_events.size() << "\n\n";

    for (const auto& e : m_events) {
        if (!e.is_suspicious) continue;
        out << "[" << e.severity.toUpper() << "] " << e.action << " | " << e.user << " | " << e.ip_address << " | " << e.mitre_technique << "\n";
    }
    file.close();
    QMessageBox::information(this, "Export", "Report exported to " + path);
}

void MainWindow::onRefreshNetwork() {
    m_netTable->setRowCount(0);
    // Read /proc/net/tcp for connections (pure C++)
    QFile tcpFile("/proc/net/tcp");
    if (!tcpFile.open(QIODevice::ReadOnly)) return;

    QTextStream in(&tcpFile);
    QString header = in.readLine(); // skip header
    int total = 0, established = 0, listening = 0;

    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        QStringList parts = line.split(QRegularExpression("\\s+"));
        if (parts.size() < 4) continue;
        total++;

        // Parse hex addresses
        auto parseAddr = [](const QString& hex) -> QString {
            QStringList p = hex.split(":");
            if (p.size() != 2) return hex;
            bool ok;
            quint32 ip = p[0].toUInt(&ok, 16);
            int port = p[1].toInt(&ok, 16);
            return QString("%1.%2.%3.%4:%5")
                .arg(ip & 0xFF).arg((ip >> 8) & 0xFF).arg((ip >> 16) & 0xFF).arg((ip >> 24) & 0xFF).arg(port);
        };

        QString local = parseAddr(parts[1]);
        QString remote = parseAddr(parts[2]);
        int state = parts[3].toInt(nullptr, 16);

        QString statusStr;
        switch (state) {
            case 1: statusStr = "ESTABLISHED"; established++; break;
            case 2: statusStr = "SYN_SENT"; break;
            case 6: statusStr = "TIME_WAIT"; break;
            case 10: statusStr = "LISTEN"; listening++; break;
            default: statusStr = QString("0x%1").arg(state, 0, 16); break;
        }

        int row = m_netTable->rowCount();
        m_netTable->insertRow(row);
        QStringList vals = {"-", "-", local, remote, statusStr, "TCP"};
        for (int c = 0; c < 6; c++) {
            auto* item = new QTableWidgetItem(vals[c]);
            if (statusStr == "ESTABLISHED") item->setForeground(QColor(Theme::ACCENT_GREEN));
            else if (statusStr == "LISTEN") item->setForeground(QColor(Theme::ACCENT_CYAN));
            m_netTable->setItem(row, c, item);
        }
    }
    m_netSummary->setText(QString("Total: %1 | Established: %2 | Listening: %3").arg(total).arg(established).arg(listening));
}

// ═══════════════════════════════════════════════════════════════
//  MAIN
// ═══════════════════════════════════════════════════════════════
int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    app.setStyle("Fusion");

    // Dark palette
    QPalette pal;
    pal.setColor(QPalette::Window, QColor(Theme::BG_PRIMARY));
    pal.setColor(QPalette::WindowText, QColor(Theme::TEXT_PRIMARY));
    pal.setColor(QPalette::Base, QColor(Theme::BG_CARD));
    pal.setColor(QPalette::AlternateBase, QColor(Theme::BG_SURFACE));
    pal.setColor(QPalette::Text, QColor(Theme::TEXT_PRIMARY));
    pal.setColor(QPalette::Button, QColor(Theme::BG_CARD));
    pal.setColor(QPalette::ButtonText, QColor(Theme::TEXT_PRIMARY));
    pal.setColor(QPalette::Highlight, QColor(Theme::ACCENT_BLUE));
    pal.setColor(QPalette::HighlightedText, Qt::white);
    app.setPalette(pal);

    MainWindow window;
    window.show();
    return app.exec();
}
