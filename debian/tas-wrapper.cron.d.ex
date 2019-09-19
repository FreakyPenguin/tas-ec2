#
# Regular cron jobs for the tas-wrapper package
#
0 4	* * *	root	[ -x /usr/bin/tas-wrapper_maintenance ] && /usr/bin/tas-wrapper_maintenance
