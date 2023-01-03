from datetime import datetime
from datetime import timedelta


def nextFriday(date: datetime, weeks_ahead: int = 0) -> datetime:
    days_ahead = 4 - date.weekday()
    if days_ahead <= 0:  # Target day already happened this week
        days_ahead += 7
    days_ahead += 7 * weeks_ahead
    return date + timedelta(days_ahead)
