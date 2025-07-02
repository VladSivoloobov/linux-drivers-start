#include <linux/module.h>     // Основные макросы и функции для модулей ядра
#include <linux/fs.h>         // Структуры и функции для работы с файловыми операциями
#include <linux/miscdevice.h> // Упрощённый способ регистрации символьных устройств
#include <linux/uaccess.h>    // Функции для безопасного копирования между ядром и пользовательским пространством

// Размер буфера сообщений (в байтах)
#define MSG_SIZE 1024

// Ядерный буфер, хранящий строку. Инициализирован начальным текстом.
static char msg[MSG_SIZE] = "Hello from misc device!\n";

/**
 * misc_read - Вызывается при чтении из устройства (например, через cat /dev/hello_misc)
 * @file: указатель на структуру файла (не используется в этом примере)
 * @buf: указатель на буфер в пользовательском пространстве, куда будут записаны данные
 * @count: количество запрошенных байтов
 * @ppos: указатель на текущее смещение в буфере (позиция чтения/записи)
 *
 * Эта функция копирует данные из ядерного буфера `msg` в пользовательский буфер `buf`.
 */
static ssize_t misc_read(struct file *file, char __user *buf, size_t count, loff_t *ppos)
{
  // Если пользователь просит 0 байт — сразу возвращаем 0 (EOF)
  if (count == 0)
    return 0;

  // Получаем длину строки в ядерном буфере
  int len = strlen(msg);

  // Если позиция за пределами строки — достигнут конец файла
  if (*ppos >= len)
  {
    *ppos = 0; // Сбрасываем позицию
    return 0;  // Возвращаем 0 — больше читать нечего
  }

  // Вычисляем, сколько байт нужно скопировать
  size_t bytes_to_copy = len - *ppos;

  // Копируем данные из ядра в пользовательское пространство
  if (copy_to_user(buf, msg + *ppos, bytes_to_copy))
    return -EFAULT; // Ошибка доступа к пользовательской памяти

  // Обновляем позицию чтения
  *ppos += bytes_to_copy;

  // Возвращаем количество успешно скопированных байт
  return bytes_to_copy;
}

/**
 * misc_write - Вызывается при записи в устройство (например, echo "test" > /dev/hello_misc)
 * @file: указатель на структуру файла (не используется в этом примере)
 * @buf: указатель на буфер в пользовательском пространстве, откуда будут взяты данные
 * @count: количество байтов, которые нужно записать
 * @ppos: указатель на текущее смещение в буфере (позиция записи)
 *
 * Эта функция копирует данные из пользовательского буфера в ядерный массив `msg`.
 */
static ssize_t misc_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos)
{
  // Ограничиваем длину записи, чтобы не выйти за границы буфера
  if (count > MSG_SIZE - 1)
    count = MSG_SIZE - 1;

  // Копируем данные из пользовательского пространства в ядерный буфер
  if (copy_from_user(msg, buf, count))
    return -EFAULT; // Ошибка доступа к пользовательской памяти

  // Добавляем завершающий ноль, чтобы получить корректную C-строку
  msg[count] = '\0';

  // Обновляем позицию записи
  *ppos += count;

  // Возвращаем количество записанных байт
  return count;
}

/**
 * file_operations - таблица файловых операций
 *
 * Здесь мы связываем системные вызовы (read, write и т.д.) с нашими функциями.
 */
static const struct file_operations fops = {
    .owner = THIS_MODULE, // Владелец — этот модуль
    .read = misc_read,    // Функция чтения
    .write = misc_write,  // Функция записи
};

/**
 * hello_misc_device - структура miscdevice
 *
 * Это упрощённый способ регистрации символьного устройства.
 * Мы регистрируем его как `/dev/hello_misc` с заданными файловыми операциями.
 */
static struct miscdevice hello_misc_device = {
    .minor = MISC_DYNAMIC_MINOR, // Ядро само выберет подходящий минорный номер
    .name = "hello_misc",        // Имя устройства — появится как /dev/hello_misc
    .fops = &fops,               // Таблица файловых операций
};

/**
 * misc_init - точка входа модуля
 * Вызывается при загрузке модуля (sudo insmod)
 */
static int __init misc_init(void)
{
  // Регистрируем наше устройство
  int err = misc_register(&hello_misc_device);

  // Если регистрация провалилась — выводим ошибку и возвращаем её код
  if (err)
  {
    printk(KERN_ERR "Failed to register misc device\n");
    return err;
  }

  // Сообщение о успешной регистрации
  printk(KERN_INFO "Misc device registered\n");

  // Успех
  return 0;
}

/**
 * misc_exit - точка выхода модуля
 * Вызывается при выгрузке модуля (sudo rmmod)
 */
static void __exit misc_exit(void)
{
  // Отменяем регистрацию устройства
  misc_deregister(&hello_misc_device);

  // Сообщение о выгрузке
  printk(KERN_INFO "Misc device unregistered\n");
}

// Указываем, какие функции вызывать при загрузке и выгрузке
module_init(misc_init);
module_exit(misc_exit);

// Информация о модуле — отображается в modinfo
MODULE_LICENSE("GPL");                                     // Лицензия GNU General Public License
MODULE_AUTHOR("Vladislav");                                // Автор
MODULE_DESCRIPTION("Simple misc character device driver"); // Описание