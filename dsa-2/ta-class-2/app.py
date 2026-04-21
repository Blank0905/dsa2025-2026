from flask import Flask, render_template, request, jsonify
from flask_cors import CORS
import pymysql
import datetime

# 🌟 LINE Bot 必備套件
from linebot import LineBotApi, WebhookHandler
from linebot.exceptions import InvalidSignatureError
from linebot.models import MessageEvent, TextMessage, TextSendMessage

app = Flask(__name__)
CORS(app)

# ====== LINE Bot 金鑰設定 ======
line_bot_api = LineBotApi('yehnqstA3qMuGSCia+xR3nyHJ/pOKMVbsKOzbazO6AXX4e34R89gU1/GzPcHrC7/UTLyMZaUWyHBELeMAt31XG9TfwV0lHf5zCQRH1HXpfZxhh/ZiUOknbM6UaNJqcyQAeMdUfierwzqIFdgaaiMNgdB04t89/1O/w1cDnyilFU=') # 請填入channel access token
handler = WebhookHandler('d373177de09002fc7ad7f0d3262790b0') # 請填入 channel secret
# ==================================


# ====== 基礎與資料庫設定 ======
@app.route('/')
def index():
    return render_template('index.html')

def get_db_connection():
    return pymysql.connect(
        host='localhost',
        user='root',
        password='', 
        database='taclass', #請填入資料庫設定 
        charset='utf8mb4',
        cursorclass=pymysql.cursors.DictCursor 
    )
# ==================================


# ====== 網頁版 API 路由 ======
@app.route('/api/get_meta', methods=['GET'])
def get_meta():
    try:
        conn = get_db_connection()
        with conn.cursor() as cursor:
            cursor.execute("SELECT user_id, user_name FROM user1")
            users = cursor.fetchall()
            cursor.execute("SELECT cat_id, cat_name FROM Categories")
            categories = cursor.fetchall()
            cursor.execute("SELECT curr_id, code, exchange_rate FROM Currencies")
            currencies = cursor.fetchall()
        return jsonify({'users': users, 'categories': categories, 'currencies': currencies})
    except Exception as e:
        return jsonify({'error': str(e)}), 500
    finally:
        if 'conn' in locals(): conn.close()

@app.route('/api/get_user_detail', methods=['GET'])
def get_user_detail():
    user_id = request.args.get('user_id')
    try:
        conn = get_db_connection()
        with conn.cursor() as cursor:
            cursor.execute("SELECT acc_id, acc_name, balance FROM Accounts WHERE user_id = %s", (user_id,))
            accounts = cursor.fetchall()
            cursor.execute("""
                SELECT t.created_at, cat.cat_name, t.description, t.amount, t.curr_id, c.code as curr_code 
                FROM Transactions t 
                JOIN Categories cat ON t.cat_id = cat.cat_id 
                JOIN Currencies c ON t.curr_id = c.curr_id 
                WHERE t.user_id = %s 
                ORDER BY t.created_at DESC
            """, (user_id,))
            transactions = cursor.fetchall()
        return jsonify({'accounts': accounts, 'transactions': transactions})
    except Exception as e:
        return jsonify({'error': str(e)}), 500
    finally:
        if 'conn' in locals(): conn.close()

@app.route('/api/add_user_with_account', methods=['POST'])
def add_user_with_account():
    user_name = request.form.get('user_name')
    acc_name = request.form.get('acc_name')
    balance = request.form.get('balance')
    conn = get_db_connection()
    try:
        with conn.cursor() as cursor:
            cursor.execute("INSERT INTO user1 (user_name, user_password) VALUES (%s, '123456')", (user_name,))
            new_user_id = cursor.lastrowid
            cursor.execute("INSERT INTO Accounts (user_id, acc_name, balance) VALUES (%s, %s, %s)", (new_user_id, acc_name, balance))
        conn.commit()
        return jsonify({'success': True})
    except Exception as e:
        conn.rollback()
        return jsonify({'error': str(e)}), 500
    finally:
        if 'conn' in locals(): conn.close()

@app.route('/api/add_transaction', methods=['POST'])
def add_transaction():
    user_id = request.form.get('user_id')
    acc_id = request.form.get('acc_id')
    curr_id = request.form.get('curr_id')
    cat_id = request.form.get('cat_id')
    amount = request.form.get('amount')
    desc = request.form.get('desc')
    conn = get_db_connection()
    try:
        with conn.cursor() as cursor:
            cursor.execute("""
                INSERT INTO Transactions (user_id, acc_id, curr_id, cat_id, amount, description) 
                VALUES (%s, %s, %s, %s, %s, %s)
            """, (user_id, acc_id, curr_id, cat_id, amount, desc))
            cursor.execute("""
                UPDATE Accounts 
                SET balance = balance - (%s * (SELECT exchange_rate FROM Currencies WHERE curr_id = %s)) 
                WHERE acc_id = %s
            """, (amount, curr_id, acc_id))
        conn.commit()
        return jsonify({'success': True})
    except Exception as e:
        conn.rollback()
        return jsonify({'error': str(e)}), 500
    finally:
        if 'conn' in locals(): conn.close()


# ====== LINE Bot 路由與處理邏輯 ======

@app.route("/callback", methods=['POST'])
def callback():
    signature = request.headers['X-Line-Signature']
    body = request.get_data(as_text=True)
    try:
        handler.handle(body, signature)
    except InvalidSignatureError:
        return "簽章驗證失敗", 400
    return 'OK'

@handler.add(MessageEvent, message=TextMessage)
def handle_message(event):
    user_msg = event.message.text.strip()
    line_user_id = event.source.user_id # 取得使用者的 LINE 身分證

    if user_msg == "使用者":
        try:
            conn = get_db_connection()
            with conn.cursor() as cursor:
                cursor.execute("SELECT user_name FROM user1 where line_id = %s", (line_user_id))
                result = cursor.fetchone()
                reply_text = f"目前使用者如下：{result['user_name']}" if result else "找不到使用者，請按新功能來創建帳戶。"
        except Exception as e: reply_text = f"連線錯誤: {str(e)}"
        finally: 
            if 'conn' in locals(): conn.close()

    elif user_msg == "記帳紀錄":
        try:
            conn = get_db_connection()
            with conn.cursor() as cursor:
                # 1. 先確認使用者身分與獲取帳戶餘額
                # 我們從 accounts 抓取目前餘額，確保即便沒交易紀錄也能顯示
                sql_user = """
                    SELECT u.user_name, a.balance, a.acc_id 
                    FROM user1 u 
                    JOIN accounts a ON u.user_id = a.user_id 
                    WHERE u.line_id = %s 
                    LIMIT 1
                """
                cursor.execute(sql_user, (line_user_id,))
                user_data = cursor.fetchone()

                if not user_data:
                    reply_text = "❌ 找不到您的帳戶，請先輸入「我是 [姓名]」來綁定。"
                else:
                    user_name = user_data['user_name']
                    current_balance = user_data['balance']
                    
                    # 2. 抓取最近 10 筆交易，並精確關聯類別與幣別
                    sql_history = """
                        SELECT c.cat_name, t.description, t.amount, curr.code 
                        FROM transactions t
                        JOIN categories c ON t.cat_id = c.cat_id
                        JOIN currencies curr ON t.curr_id = curr.curr_id
                        WHERE t.user_id = (SELECT user_id FROM user1 WHERE line_id = %s)
                        ORDER BY t.created_at DESC
                        LIMIT 10
                    """
                    cursor.execute(sql_history, (line_user_id,))
                    records = cursor.fetchall()

                    # 3. 組合回覆內容
                    reply_text = f"💰【{user_name} 的記帳紀錄】\n"
                    
                    if records:
                        detail_list = []
                        for row in records:
                            item = f"* {row['cat_name']}: {row['description']} ({row['amount']} {row['code']})"
                            detail_list.append(item)
                        reply_text += "\n".join(detail_list)
                    else:
                        reply_text += "（目前尚無消費紀錄）"
                    
                    # 4. 最後補上目前餘額
                    reply_text += f"\n\n💵 目前總餘額：{current_balance} TWD"
                    
        except Exception as e:
            reply_text = f"查詢失敗，請檢查資料庫連結: {str(e)}"
        finally:
            if 'conn' in locals(): conn.close()

    elif user_msg == "新功能":
        #pass # 進階實作
        reply_text = "1.輸入「我是 您的姓名」來連結您的帳號。（例如：我是 王大明）\n" \
        "2.輸入「記帳」來看記帳的方法"

    elif user_msg.startswith("我是"):
        target_name = user_msg[2:].strip()
        try:
            conn = get_db_connection()
            with conn.cursor() as cursor:
                # 1. 檢查目前的 LINE ID 是否已經綁定過
                cursor.execute("SELECT user_name FROM user1 WHERE line_id = %s", (line_user_id,))
                already_bound = cursor.fetchone()
                
                if already_bound:
                    reply_text = f"❌ 您已經綁定過帳號「{already_bound['user_name']}」了。"
                
                else:
                    # 2. 檢查該姓名是否存在
                    cursor.execute("SELECT user_id, line_id FROM user1 WHERE user_name = %s", (target_name,))
                    target_user = cursor.fetchone()
                    
                    if target_user:
                        # 姓名存在，檢查是否被別人佔用
                        if target_user['line_id']:
                            reply_text = f"❌ 「{target_name}」已被他人綁定，請輸入其他名稱。"
                        else:
                            # 進行舊帳號綁定
                            cursor.execute("UPDATE user1 SET line_id = %s WHERE user_name = %s", (line_user_id, target_name))
                            conn.commit()
                            reply_text = f"✅ 歡迎回來，{target_name}！已成功連結您的既有帳號。"
                    else:
                        # 3. 姓名不存在，自動創建新使用者
                        # user_id 為自動遞增，password 預設為空字串
                        sql_insert_user = "INSERT INTO user1 (user_name, line_id, user_password) VALUES (%s, %s, '')"
                        cursor.execute(sql_insert_user, (target_name, line_user_id))
                        
                        # 取得新建立的 user_id 以便建立預設帳戶
                        new_user_id = conn.insert_id()
                        
                        # 4. 同步為新用戶建立一個預設帳號（例如：現金帳戶），否則他無法記帳
                        sql_insert_acc = "INSERT INTO accounts (user_id, acc_name, balance) VALUES (%s, %s, 0)"
                        cursor.execute(sql_insert_acc, (new_user_id, f"{target_name}的現金帳戶"))
                        
                        conn.commit()
                        reply_text = f"🎉 歡迎新朋友 {target_name}！系統已為您建立帳號與預設錢包，現在可以開始記帳囉！"
                        
        except Exception as e:
            reply_text = f"系統處理失敗: {str(e)}"
        finally:
            if 'conn' in locals(): conn.close()

    # 情況 A：使用者只輸入「記帳」二字，提供詳細格式說明
    elif user_msg == "記帳":
        reply_text = (
            "📝 【記帳指令教學】\n\n"
            "您可以選擇以下兩種格式：\n"
            "1️⃣ 完整格式 (自行指定類別)\n"
            "   格式：記帳 [類別] [項目] [金額] [幣別]\n"
            "   範例：記帳 飲食 午餐 120 TWD\n\n"
            "2️⃣ 簡略格式 (自動歸類為其他)\n"
            "   格式：記帳 [項目] [金額] [幣別]\n"
            "   範例：記帳 買書 500 TWD\n\n"
            "💡 目前系統支援類別：飲食、交通、娛樂、購物、其他。"
        )

    # 情況 B：使用者輸入「記帳 ...」但需要解析參數
    elif user_msg.startswith("記帳 "):
        parts = user_msg.split()
        
        # 1. 參數解析邏輯
        if len(parts) == 5:
            _, cat_input, desc, amount_str, curr_input = parts
        elif len(parts) == 4:
            _, desc, amount_str, curr_input = parts
            cat_input = "其他"
        else:
            reply_text = "⚠️ 格式錯誤！請參考：記帳 [類別] 項目 金額 幣別"
            line_bot_api.reply_message(event.reply_token, TextSendMessage(text=reply_text))
            return

        try:
            # 轉換金額為數字
            amount = float(amount_str)
            # 強制將幣別轉換為大寫 (例如: usd -> USD)
            curr_code = curr_input.upper()

            conn = get_db_connection()
            with conn.cursor() as cursor:
                # 2. 確認使用者身分
                cursor.execute("SELECT user_id FROM user1 WHERE line_id = %s", (line_user_id,))
                user = cursor.fetchone()
                if not user:
                    reply_text = "❌ 請先輸入「我是 [姓名]」綁定帳號。"
                else:
                    user_id = user['user_id']
                    
                    # 3. 查詢幣別匯率 (使用大寫 curr_code)
                    cursor.execute("SELECT curr_id, exchange_rate FROM currencies WHERE code = %s", (curr_code,))
                    curr_res = cursor.fetchone()
                    
                    if curr_res:
                        curr_id = curr_res['curr_id']
                        rate = float(curr_res['exchange_rate'])
                    else:
                        # 如果輸入的幣別資料庫沒有，預設 TWD 匯率 1.0
                        curr_id = 1
                        rate = 1.0
                        curr_code = "TWD (預設)"

                    # 4. 查詢類別
                    cursor.execute("SELECT cat_id FROM categories WHERE cat_name = %s", (cat_input,))
                    cat_res = cursor.fetchone()
                    cat_id = cat_res['cat_id'] if cat_res else 5 
                    
                    # 5. 取得帳戶資料
                    cursor.execute("SELECT acc_id FROM accounts WHERE user_id = %s LIMIT 1", (user_id,))
                    acc_res = cursor.fetchone()
                    
                    if acc_res:
                        acc_id = acc_res['acc_id']
                        
                        # A. 寫入原始交易紀錄
                        sql_insert = "INSERT INTO transactions (user_id, acc_id, cat_id, curr_id, amount, description) VALUES (%s, %s, %s, %s, %s, %s)"
                        cursor.execute(sql_insert, (user_id, acc_id, cat_id, curr_id, amount, desc))

                        # B. 扣除換算後的台幣餘額 (金額 * 匯率)
                        sql_update_balance = "UPDATE accounts SET balance = balance - (%s * %s) WHERE acc_id = %s"
                        cursor.execute(sql_update_balance, (amount, rate, acc_id))

                        conn.commit()

                        # 6. 查詢最新餘額回報給使用者
                        cursor.execute("SELECT balance FROM accounts WHERE acc_id = %s", (acc_id,))
                        new_balance = cursor.fetchone()['balance']

                        reply_text = f"✅ 記帳成功！\n項目：{desc}\n金額：{amount} {curr_code}\n匯率：{rate}\n\n💰 帳戶餘額：\n{new_balance} TWD"
                    else:
                        reply_text = "❌ 找不到您的帳戶資料。"
        except Exception as e:
            reply_text = f"處理失敗: {str(e)}"
        finally:
            if 'conn' in locals(): conn.close()
    else:
        reply_text = "指令無效"

    line_bot_api.reply_message(event.reply_token, TextSendMessage(text=reply_text))

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000, debug=True)