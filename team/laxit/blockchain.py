import hashlib
import json
import time

class Block:
    def __init__(self, index, previous_hash, timestamp, data, nonce=0):
        self.index = index
        self.previous_hash = previous_hash
        self.timestamp = timestamp
        self.data = data
        self.nonce = nonce
        self.hash = self.calculate_hash()

    def calculate_hash(self):
        value = str(self.index) + str(self.previous_hash) + str(self.timestamp) + json.dumps(self.data) + str(self.nonce)
        return hashlib.sha256(value.encode('utf-8')).hexdigest()

    def to_dict(self):
        return {
            "index": self.index,
            "previous_hash": self.previous_hash,
            "timestamp": self.timestamp,
            "data": self.data,
            "nonce": self.nonce,
            "hash": self.hash
        }

class Blockchain:
    def __init__(self, difficulty=2):
        self.chain = []
        self.difficulty = difficulty
        self.create_genesis_block()

    def create_genesis_block(self):
        genesis_block = Block(0, "0", time.time(), "Genesis Block")
        genesis_block.hash = genesis_block.calculate_hash()
        self.chain.append(genesis_block)

    def get_latest_block(self):
        return self.chain[-1]

    def mine_block(self, block):
        target = "0" * self.difficulty
        while block.hash[:self.difficulty] != target:
            block.nonce += 1
            block.hash = block.calculate_hash()
        print(f"Block Mined: {block.hash}")

    def add_event(self, event_data):
        """Adds a threat event to the ledger with Proof-of-Work."""
        latest = self.get_latest_block()
        new_block = Block(latest.index + 1, latest.hash, time.time(), event_data)
        self.mine_block(new_block)
        self.chain.append(new_block)
        return new_block

    def is_chain_valid(self):
        """Cryptographically verifies the entire ledger chain."""
        for i in range(1, len(self.chain)):
            current = self.chain[i]
            previous = self.chain[i-1]

            if current.hash != current.calculate_hash():
                return False
            if current.previous_hash != previous.hash:
                return False
        return True

    def export_ledger(self, filepath):
        with open(filepath, 'w') as f:
            json.dump([b.to_dict() for b in self.chain], f, indent=4)
