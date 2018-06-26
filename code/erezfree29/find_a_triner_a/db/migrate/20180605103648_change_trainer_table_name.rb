class ChangeTrainerTableName < ActiveRecord::Migration[5.2]
  def change
    rename_table :trainers, :training_services
  end
end
