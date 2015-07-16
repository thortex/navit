# try to get to the town search
sleep 1; xdotool key Return
sleep 1; xdotool key Down
sleep 1; xdotool key Return
sleep 1; xdotool key Down
sleep 1; xdotool key Down
sleep 1; xdotool key Return
import -window root $CIRCLE_ARTIFACTS/internal_gui_test.png
sleep 1; xdotool key M
sleep 1; xdotool key U
import -window root $CIRCLE_ARTIFACTS/internal_gui_test_munchen.png
