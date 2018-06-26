class CreateTrainers < ActiveRecord::Migration[5.2]
  def change
    create_table :trainers do |t|
      t.integer :hourly_rate
      t.references :user, foreign_key: true
      t.references :expertise, foreign_key: true

      t.timestamps
    end
  end
end
