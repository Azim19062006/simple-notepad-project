#include "simple_game_dialog.h"

#include <QKeyEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QTimer>
#include <QVBoxLayout>

namespace {
constexpr int kGameWidth = 760;
constexpr int kGameHeight = 520;
constexpr qreal kPaddleWidth = 110.0;
constexpr qreal kPaddleHeight = 16.0;
constexpr qreal kBallRadius = 8.0;
constexpr qreal kPaddleSpeed = 34.0;
constexpr int kBrickRows = 5;
constexpr int kBrickColumns = 10;
}

SimpleBreakoutWidget::SimpleBreakoutWidget(QWidget* parent)
    : QWidget(parent)
    , timer_(new QTimer(this))
    , score_(0)
    , lives_(3)
    , running_(false)
    , gameOver_(false)
    , victory_(false)
{
    setFixedSize(kGameWidth, kGameHeight);
    setFocusPolicy(Qt::StrongFocus);
    setMouseTracking(true);

    connect(timer_, &QTimer::timeout, this, &SimpleBreakoutWidget::tick);
    timer_->start(16);
    resetGame();
}

void SimpleBreakoutWidget::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter
        || event->key() == Qt::Key_Space) {
        if (gameOver_ || victory_) {
            resetGame();
        }
        running_ = true;
        return;
    }

    if (event->key() == Qt::Key_Left || event->key() == Qt::Key_A) {
        movePaddle(-kPaddleSpeed);
        return;
    }

    if (event->key() == Qt::Key_Right || event->key() == Qt::Key_D) {
        movePaddle(kPaddleSpeed);
        return;
    }

    QWidget::keyPressEvent(event);
}

void SimpleBreakoutWidget::mouseMoveEvent(QMouseEvent* event)
{
    qreal x = event->position().x() - paddle_.width() / 2.0;
    x = qBound(0.0, x, width() - paddle_.width());
    paddle_.moveLeft(x);

    if (!running_) {
        ballPosition_.setX(paddle_.center().x());
    }
    update();
}

void SimpleBreakoutWidget::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillRect(rect(), QColor(18, 24, 32));

    QLinearGradient headerGradient(0, 0, width(), 0);
    headerGradient.setColorAt(0.0, QColor(51, 136, 255));
    headerGradient.setColorAt(1.0, QColor(42, 202, 151));
    painter.fillRect(QRectF(0, 0, width(), 42), headerGradient);

    painter.setPen(Qt::white);
    painter.setFont(QFont("Helvetica", 13, QFont::Bold));
    painter.drawText(18, 27, QString("Score: %1").arg(score_));
    painter.drawText(width() - 100, 27, QString("Lives: %1").arg(lives_));

    const QVector<QColor> rowColors = {
        QColor(255, 94, 98),
        QColor(255, 166, 77),
        QColor(255, 220, 92),
        QColor(76, 201, 240),
        QColor(116, 198, 157),
    };

    painter.setPen(Qt::NoPen);
    for (int i = 0; i < bricks_.size(); ++i) {
        int row = i / kBrickColumns;
        painter.setBrush(rowColors[row % rowColors.size()]);
        painter.drawRoundedRect(bricks_[i], 4, 4);
    }

    painter.setBrush(QColor(238, 242, 245));
    painter.drawRoundedRect(paddle_, 7, 7);

    painter.setBrush(QColor(255, 255, 255));
    painter.drawEllipse(ballPosition_, kBallRadius, kBallRadius);

    painter.setPen(QColor(226, 232, 240));
    painter.setFont(QFont("Helvetica", 12));
    if (!running_ && !gameOver_ && !victory_) {
        drawCenteredText(painter, "Press Enter or Space to start", height() - 38);
    } else if (gameOver_) {
        painter.setFont(QFont("Helvetica", 24, QFont::Bold));
        drawCenteredText(painter, "Game Over", height() / 2.0 - 10);
        painter.setFont(QFont("Helvetica", 12));
        drawCenteredText(painter, "Press Enter to restart", height() / 2.0 + 24);
    } else if (victory_) {
        painter.setFont(QFont("Helvetica", 24, QFont::Bold));
        drawCenteredText(painter, "You Win!", height() / 2.0 - 10);
        painter.setFont(QFont("Helvetica", 12));
        drawCenteredText(painter, "Press Enter to play again", height() / 2.0 + 24);
    }
}

void SimpleBreakoutWidget::tick()
{
    if (running_ && !gameOver_ && !victory_) {
        updateBall();
    }
    update();
}

void SimpleBreakoutWidget::resetGame()
{
    score_ = 0;
    lives_ = 3;
    running_ = false;
    gameOver_ = false;
    victory_ = false;

    paddle_ = QRectF((width() - kPaddleWidth) / 2.0, height() - 48,
        kPaddleWidth, kPaddleHeight);
    buildBricks();
    resetBall();
    update();
}

void SimpleBreakoutWidget::resetBall()
{
    running_ = false;
    ballPosition_ = QPointF(paddle_.center().x(), paddle_.top() - kBallRadius - 2);
    ballVelocity_ = QPointF(4.0, -4.5);
}

void SimpleBreakoutWidget::buildBricks()
{
    bricks_.clear();

    qreal gap = 8.0;
    qreal left = 38.0;
    qreal top = 72.0;
    qreal brickWidth = (width() - left * 2.0 - gap * (kBrickColumns - 1)) / kBrickColumns;
    qreal brickHeight = 22.0;

    for (int row = 0; row < kBrickRows; ++row) {
        for (int column = 0; column < kBrickColumns; ++column) {
            qreal x = left + column * (brickWidth + gap);
            qreal y = top + row * (brickHeight + gap);
            bricks_.append(QRectF(x, y, brickWidth, brickHeight));
        }
    }
}

void SimpleBreakoutWidget::movePaddle(qreal amount)
{
    qreal x = paddle_.left() + amount;
    x = qBound(0.0, x, width() - paddle_.width());
    paddle_.moveLeft(x);

    if (!running_) {
        ballPosition_.setX(paddle_.center().x());
    }
    update();
}

void SimpleBreakoutWidget::updateBall()
{
    ballPosition_ += ballVelocity_;

    if (ballPosition_.x() - kBallRadius <= 0 || ballPosition_.x() + kBallRadius >= width()) {
        ballVelocity_.setX(-ballVelocity_.x());
    }
    if (ballPosition_.y() - kBallRadius <= 42) {
        ballVelocity_.setY(qAbs(ballVelocity_.y()));
    }

    QRectF ballRect(ballPosition_.x() - kBallRadius, ballPosition_.y() - kBallRadius,
        kBallRadius * 2.0, kBallRadius * 2.0);

    if (ballVelocity_.y() > 0 && ballRect.intersects(paddle_)) {
        qreal offset = (ballPosition_.x() - paddle_.center().x()) / (paddle_.width() / 2.0);
        ballVelocity_.setX(offset * 5.0);
        ballVelocity_.setY(-qAbs(ballVelocity_.y()));
        ballPosition_.setY(paddle_.top() - kBallRadius - 1);
    }

    for (int i = 0; i < bricks_.size(); ++i) {
        if (ballRect.intersects(bricks_[i])) {
            bricks_.removeAt(i);
            score_ += 10;
            ballVelocity_.setY(-ballVelocity_.y());
            break;
        }
    }

    if (bricks_.isEmpty()) {
        running_ = false;
        victory_ = true;
    }

    if (ballPosition_.y() - kBallRadius > height()) {
        --lives_;
        if (lives_ <= 0) {
            running_ = false;
            gameOver_ = true;
        } else {
            resetBall();
        }
    }
}

void SimpleBreakoutWidget::drawCenteredText(QPainter& painter, const QString& text, qreal y)
{
    QRectF textRect(0, y - 24, width(), 48);
    painter.drawText(textRect, Qt::AlignCenter, text);
}

SimpleGameDialog::SimpleGameDialog(QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle("Simple Game");
    setModal(false);

    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(new SimpleBreakoutWidget(this));
}
