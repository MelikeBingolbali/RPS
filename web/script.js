document.addEventListener("DOMContentLoaded", () => {
    const playerScoreElement = document.querySelector(".p-count");
    const computerScoreElement = document.querySelector(".c-count");
    const movesLeftElement = document.querySelector(".movesleft");
    const resultElement = document.querySelector(".result");
    const playerInfoElement = document.querySelector(".move");
    const reloadButton = document.querySelector(".reload");
    const chatBody = document.querySelector("#chat-body");
    const chatInput = document.querySelector("#chat-input");

    let playerMoveMade = false; // Oyuncunun hamle yapıp yapmadığını takip eder
    let playerRole = "Unknown Player"; // Oyuncunun rolü

    // Restart butonunu başlangıçta gizle
    reloadButton.style.display = "none";
    reloadButton.disabled = true;

    const socket = new WebSocket('ws://localhost:8080');

    socket.addEventListener('open', function () {
        console.log('Connected to the WebSocket server');
        playerInfoElement.innerText = "Connecting...";
    });

    socket.addEventListener('message', function (event) {
        const message = event.data;
        console.log('Message from server:', message);

        try {
            const parsedMessage = JSON.parse(message);

            if (parsedMessage.result) {
                // Kazanan mesajlarını eklemeden önce var olan mesajları temizleyelim
                const winLoseMessage = document.querySelector(".win-lose-message");
                if (winLoseMessage) winLoseMessage.remove();

                playerScoreElement.innerText = parsedMessage.player1_score;
                computerScoreElement.innerText = parsedMessage.player2_score;
                movesLeftElement.innerText = `Moves Left: ${parsedMessage.moves_left}`;

                // Kazanan oyuncunun ismini ve kazanma/kaybetme mesajını ekleyelim
                if (parsedMessage.result.includes("Player 1 wins")) {
                resultElement.innerText = parsedMessage.result;
                } else if (parsedMessage.result.includes("Player 2 wins")) {
                resultElement.innerText = parsedMessage.result;
                }

                if (parsedMessage.game_over) {
                    playerInfoElement.innerText = "Game Over!";
                    reloadButton.style.display = "block"; // Restart butonunu göster
                    reloadButton.disabled = false; // Restart butonunu etkinleştir

                    // Taş, kağıt, makas butonlarını gizle
                    disableButtons();
                    hideButtons();
                }

                playerMoveMade = false; // Oyuncunun hamle durumu sıfırlanır
                enableButtons();
            } else if (parsedMessage.role) {
                playerRole = parsedMessage.role || "Unknown Player";
                playerInfoElement.innerText = `You are ${playerRole}`;
            } else if (parsedMessage.restart) {
                location.reload(); // Sayfayı yeniden yükleyerek oyunu yeniden başlat
            } else if (parsedMessage.waiting) {
                playerInfoElement.innerText = `Waiting for ${parsedMessage.waiting}`;
                disableButtons();
            } else if (parsedMessage === "Game starting") {
                enableButtons();
                reloadButton.style.display = "none"; // Restart butonunu gizle
                reloadButton.disabled = true; // Restart butonunu devre dışı bırak
            } else if (parsedMessage.play) { 
                if (playerInfoElement.innerText !== "Game Over!"){
                    playerInfoElement.innerText = 'Choose one of them';
                }
                enableButtons();
                playerMoveMade = false;
            } else {
                // Skorlar ve hamle bilgileri güncellenir
                if (parsedMessage.player1_score !== undefined) {
                    playerScoreElement.innerText = parsedMessage.player1_score;
                }
                if (parsedMessage.player2_score !== undefined) {
                    computerScoreElement.innerText = parsedMessage.player2_score;
                }
                if (parsedMessage.moves_left !== undefined) {
                    movesLeftElement.innerText = `Moves Left: ${parsedMessage.moves_left}`;
                }
            }

            enableButtons();
        } catch (e) {
            console.error('Failed to parse message:', e);
        }
    });

    socket.addEventListener('close', function () {
        console.log('Disconnected from WebSocket server');
        playerInfoElement.innerText = "Disconnected";
    });

    socket.addEventListener('error', function (error) {
        console.error('WebSocket error:', error);
        playerInfoElement.innerText = "Connection Error";
    });

    function sendMove(move) {
        console.log('Sending move:', move);
        if (!playerMoveMade) {
            const moveData = JSON.stringify({ move });
            socket.send(moveData);
            playerMoveMade = true; // Oyuncunun hamle yaptığını işaretle
            disableButtons(); // Hamle yapıldıktan sonra butonları devre dışı bırak
        }
    }

    function restartGame() {
        console.log('Game restarting...');
        reloadButton.style.display = "none"; // Restart butonunu tamamen gizle
        reloadButton.disabled = true;
        enableButtons();
    }

    function disableButtons() {
        const buttons = document.querySelectorAll(".rock, .paper, .scissor");
        buttons.forEach(button => {
            button.disabled = true;
            button.style.backgroundColor = "darkgray";
        });
    }

    function enableButtons() {
        const buttons = document.querySelectorAll(".rock, .paper, .scissor");
        buttons.forEach(button => {
            button.removeAttribute("disabled");
            button.style.backgroundColor = "";
        });
    }

    function hideButtons() {
        const buttons = document.querySelectorAll(".rock, .paper, .scissor");
        buttons.forEach(button => {
            button.style.display = "none";
        });
    }

    document.querySelector(".rock").addEventListener("click", () => sendMove("rock"));
    document.querySelector(".paper").addEventListener("click", () => sendMove("paper"));
    document.querySelector(".scissor").addEventListener("click", () => sendMove("scissors"));

    reloadButton.addEventListener("click", () => {
        const restartData = JSON.stringify({ restart: true });
        socket.send(restartData);
        location.reload(); // Sayfayı yenile
    });
});
